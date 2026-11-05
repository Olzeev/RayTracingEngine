#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Tile.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Flex.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Tree.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <iostream>
#include <filesystem>


namespace fs = std::filesystem;

int TOP_PANEL_HEIGHT = 80;
int TOP_PANEL_MIN_SIZE[2] = {200, 100};

int LEFT_PANEL_WIDTH = 200;
int LEFT_PANEL_MIN_SIZE[2] = {100, 200};

int RIGHT_PANEL_WIDTH = 200;
int RIGHT_PANEL_MIN_SIZE[2] = {100, 200};

int BOTTOM_PANEL_HEIGHT = 100;
int BOTTOM_PANEL_MIN_SIZE[2] = {200, 100};

int CENTRAL_PANEL_MIN_SIZE[2] = {100, 100};

int WINDOW_WIDTH = 1280; 
int WINDOW_HEIGHT = 720;
int WINDOW_MIN_SIZE[2] = {
    std::max(TOP_PANEL_MIN_SIZE[0], LEFT_PANEL_MIN_SIZE[0] + RIGHT_PANEL_MIN_SIZE[0] + CENTRAL_PANEL_MIN_SIZE[0]), 
    std::max(TOP_PANEL_MIN_SIZE[1] + LEFT_PANEL_MIN_SIZE[1] + BOTTOM_PANEL_MIN_SIZE[1], TOP_PANEL_MIN_SIZE[1] + BOTTOM_PANEL_MIN_SIZE[1] + CENTRAL_PANEL_MIN_SIZE[1])
};

std::string OPENED_DIRECTORY = "";

void build_file_tree(Fl_Tree & tree, const fs::path& directory) {
    // TODO: implement lazy loading
    // TODO: sort entries

    tree.clear();
    std::string root_name = directory.filename();
    if (root_name.empty())
        root_name = directory.string();

    OPENED_DIRECTORY = root_name;
    tree.root_label(root_name.c_str());

    std::error_code error;
    fs::recursive_directory_iterator iterator(
        directory,
        fs::directory_options::skip_permission_denied,
        error
    );

    for (const auto& entry : iterator) {
        fs::path relative_path = fs::relative(entry.path(), directory, error);

        if (!error) {
            Fl_Tree_Item* cur_item = tree.add(relative_path.generic_string().c_str());
            cur_item->close();
        }

        error.clear();
    }

    tree.redraw();
}

void open_project(Fl_Widget* w, void * data) {
    Fl_Native_File_Chooser chooser;
    chooser.title("Выберите папку");
    chooser.type(Fl_Native_File_Chooser::BROWSE_DIRECTORY);
    switch (chooser.show()) {
        case -1:
            std::cerr << "ERROR: " << chooser.errmsg();
            break;
        case 1:
            break;
        default:
            auto* file_tree = static_cast<Fl_Tree*>(data);
            build_file_tree(*file_tree, chooser.filename());
            break;
    }
}

int main(int argc, char **argv) {
     Fl::option(Fl::OPTION_FNFC_USES_GTK, false);
    Fl_Window *window = new Fl_Window(WINDOW_WIDTH, WINDOW_HEIGHT);

    Fl_Group *top_panel = new Fl_Group(0,0, WINDOW_WIDTH, TOP_PANEL_HEIGHT);
    top_panel->box(FL_BORDER_BOX);
    top_panel->labelfont(FL_BOLD+FL_ITALIC);
    top_panel->labelsize(36);

    Fl_Menu_Bar menu_bar1(0, 0, WINDOW_WIDTH, 40);
    


    Fl_Flex flex2(5, 45, WINDOW_WIDTH, 30, Fl_Flex::HORIZONTAL);
    
    Fl_Button bt1(0, 0, 10, 30, "@>");
    Fl_Button bt2(0, 0, 10, 30, "@square");
    flex2.gap(10);
    flex2.fixed(bt1, 60);
    flex2.fixed(bt2, 60);
    flex2.end();
    top_panel->resizable(flex2);
    top_panel->end();
    Fl_Tile *tile = new Fl_Tile(0, TOP_PANEL_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT - TOP_PANEL_HEIGHT);

    Fl_Group *left_panel = new Fl_Group(0, TOP_PANEL_HEIGHT, LEFT_PANEL_WIDTH, WINDOW_HEIGHT - TOP_PANEL_HEIGHT - BOTTOM_PANEL_HEIGHT);
    left_panel->box(FL_BORDER_BOX);
    
    Fl_Tree file_tree(left_panel->x(), left_panel->y(), left_panel->w(), left_panel->h());

    menu_bar1.add("Файл/Открыть", 0, open_project, &file_tree);
    menu_bar1.add("Файл/Создать");

    left_panel->end();
    
    Fl_Group *central_panel = new Fl_Group(LEFT_PANEL_WIDTH, TOP_PANEL_HEIGHT, 
        WINDOW_WIDTH - LEFT_PANEL_WIDTH - RIGHT_PANEL_WIDTH, WINDOW_HEIGHT - TOP_PANEL_HEIGHT - BOTTOM_PANEL_HEIGHT);
    central_panel->box(FL_BORDER_BOX);
    central_panel->labelfont(FL_BOLD+FL_ITALIC);
    central_panel->labelsize(36);

    central_panel->end();

    Fl_Group *right_panel = new Fl_Group(WINDOW_WIDTH - RIGHT_PANEL_WIDTH, TOP_PANEL_HEIGHT, 
        RIGHT_PANEL_WIDTH, WINDOW_HEIGHT - TOP_PANEL_HEIGHT - BOTTOM_PANEL_HEIGHT);
    right_panel->box(FL_BORDER_BOX);
    right_panel->labelfont(FL_BOLD+FL_ITALIC);
    right_panel->labelsize(36);

    right_panel->end();
    

    Fl_Group *bottom_panel = new Fl_Group(0, WINDOW_HEIGHT - BOTTOM_PANEL_HEIGHT, WINDOW_WIDTH, BOTTOM_PANEL_HEIGHT);
    bottom_panel->box(FL_BORDER_BOX);
    bottom_panel->labelfont(FL_BOLD+FL_ITALIC);
    bottom_panel->labelsize(36);

    bottom_panel->end();
    
    tile->end();

    tile->size_range(right_panel, RIGHT_PANEL_MIN_SIZE[0], RIGHT_PANEL_MIN_SIZE[1]);
    tile->size_range(top_panel, TOP_PANEL_MIN_SIZE[0], TOP_PANEL_MIN_SIZE[1]);
    tile->size_range(left_panel, LEFT_PANEL_MIN_SIZE[0], LEFT_PANEL_MIN_SIZE[1]);
    tile->size_range(bottom_panel, BOTTOM_PANEL_MIN_SIZE[0], BOTTOM_PANEL_MIN_SIZE[1]);
    tile->size_range(central_panel, CENTRAL_PANEL_MIN_SIZE[0], CENTRAL_PANEL_MIN_SIZE[1]);

    window->end();
    
    
    window->size_range(WINDOW_MIN_SIZE[0], WINDOW_MIN_SIZE[1]);

    window->resizable(tile);


    window->show(argc, argv);
    return Fl::run();
}