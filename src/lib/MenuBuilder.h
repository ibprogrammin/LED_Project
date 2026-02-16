#ifndef MENUBUILDER_H
#define MENUBUILDER_H

#include <string>
#include <utility>
#include <vector>

struct MenuItem {
    public:
    int id;
    std::string label;
    MenuItem* parent;
    std::vector<MenuItem> children;

    MenuItem() : id(0), label(""), parent(nullptr) {}
    MenuItem(int id, std::string label, MenuItem* parent) 
        : id(id), label(std::move(label)), parent(parent) {}

    void addChild(MenuItem child) {
        this->children.push_back(std::move(child));
    }

};

class MenuBuilder {
    public:
        MenuBuilder();
        void init();
        void updateMenu();
        void moveMenuUp();
        void moveMenuDown();
        void addMenuItem(MenuItem *parent, std::string label);
        static std::vector<std::string> getList(const std::string& prefix, const std::vector<MenuItem>& children);
        std::vector<std::string> getDisplayBuffer() const { 
            return std::vector<std::string>(display_buffer, display_buffer + 2); 
        }

    private:
        //std::vector<MenuItem> menu_items;
        MenuItem root_menu_item;
        std::vector<std::string> menu_list;
        std::string display_buffer[2];
        int current_menu_index = 0;
};

#endif
