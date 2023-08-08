#version 130

mat2 rot(float a) {
	float s = sin(a);
	float c = cos(a);
	return mat2(c, -s, s, c);
}

vec2 sphIntersect( in vec3 ro, in vec3 rd, in vec3 ce, float ra )
{
    vec3 oc = ro - ce;
    float b = dot( oc, rd );
    vec3 qc = oc - b*rd;
    float h = ra*ra - dot( qc, qc );
    if( h<0.0 ) return vec2(-1.0); // no intersection
    h = sqrt( h );
    return vec2( -b-h, -b+h );
}

vec2 boxIntersection( in vec3 ro, in vec3 rd, vec3 boxSize, out vec3 outNormal ) 
{
    vec3 m = 1.0/rd; // can precompute if traversing a set of aligned boxes
    vec3 n = m*ro;   // can precompute if traversing a set of aligned boxes
    vec3 k = abs(m)*boxSize;
    vec3 t1 = -n - k;
    vec3 t2 = -n + k;
    float tN = max( max( t1.x, t1.y ), t1.z );
    float tF = min( min( t2.x, t2.y ), t2.z );
    if( tN>tF || tF<0.0) return vec2(-1.0); // no intersection
    outNormal = (tN>0.0) ? step(vec3(tN),t1) : // ro ouside the box
                           step(t2,vec3(tF));  // ro inside the box
    outNormal *= -sign(rd);
    return vec2( tN, tF );
}

struct Sphere{
    vec3 pos;
    float r;
};

struct Box{
    vec3 pos;
    vec3 size;
};

out vec4 FragColor;

uniform vec3 player_pos;
uniform vec3 player_mouse_position;
uniform vec2 resolution;

const int spheres_cnt = 0;
//Sphere spheres[spheres_cnt] = Sphere[]();

const int boxes_cnt = 1;
Box boxes[boxes_cnt] = Box[](Box(vec3(10, 0, 0), vec3(5, 5, 5)));

void main(){
    vec2 frag_coord = (gl_TexCoord[0].xy - 0.5) * resolution / resolution.y;
    frag_coord.y *= -1;
    vec3 ray_dir = normalize(vec3(1, 0, 0) + vec3(0, frag_coord.y, -frag_coord.x));
	//ray_dir.zx *= rot(-player_mouse_position.y);
	//ray_dir.xy *= rot(player_mouse_position.x);


    FragColor = vec4(0.5, 0.8, 0.9, 1);
    /*
    if (spheres_cnt > 0){
        for (int i = 0; i < spheres_cnt; ++i){
            vec2 dist_cur = sphIntersect(player_pos, ray_dir, spheres[i].pos, spheres[i].r);
            if (dist_cur.x != -1){
                FragColor = vec4(1);
            }
        }
    }
    
    */
    
    for (int i = 0; i < boxes_cnt; ++i){
        vec3 normal;
        vec2 dist_cur = boxIntersection(player_pos, ray_dir, boxes[i].size, normal);
        if (dist_cur.x != -1){
            FragColor = vec4(1);
        }
    }
}