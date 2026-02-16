/// MenuBuilder.cpp
// Created by Daniel on 2024-06-16.
// Implementation of the MenuBuilder class
// This class is responsible for building and managing a hierarchical menu structure for the LED controller's LCD display.
// The MenuBuilder class provides methods to initialize the menu, navigate through it, and retrieve the current menu items for display.
// The menu structure is defined using a tree of MenuItem objects, where each MenuItem can have multiple child items, allowing for nested submenus.
// The MenuBuilder class also includes a method to generate a flat list of menu items with appropriate prefixes to indicate their hierarchical level, which can be used for displaying the menu on the LCD screen.
// First: Call init() to set up the default menu structure
// Then: Use moveMenuUp() and moveMenuDown() to navigate through the menu items
// Finally: Call getDisplayBuffer() to retrieve the current menu items for display on the LCD screen. The display buffer will contain the current menu item and the next menu item for easy navigation.

#include "MenuBuilder.h"

namespace {

    // Helper function to count the total number of nodes in the menu tree, used for reserving space in the list vector
    size_t countNodes(const std::vector<MenuItem>& children) {
        size_t count = 0;
        for (const MenuItem& child : children) {
            count += 1;
            count += countNodes(child.children);
        }
        return count;
    }
    
    //list of menu items with prefixes to indicate their hierarchical level
    void buildList(std::string& prefix, const std::vector<MenuItem>& children, std::vector<std::string>& out) {
        for (const MenuItem& child : children) {
        out.emplace_back(prefix);
        out.back() += child.label;
            prefix.push_back('-');
            buildList(prefix, child.children, out);
            prefix.pop_back();
        }
    }
}

// Include the header file for the MenuBuilder class1111
MenuBuilder::MenuBuilder() {
}

// init method to set up the default menu structure
void MenuBuilder::init() {
    root_menu_item = MenuItem(0, "Main Menu", nullptr);
    
    MenuItem item_2 = MenuItem(1, "Power", &root_menu_item);
    
    MenuItem item_3 = MenuItem(2, "Color Source", &root_menu_item);
    item_3.addChild(MenuItem(3, "Controls", &item_3));
    item_3.addChild(MenuItem(4, "Bluetooth", &item_3));
    item_3.addChild(MenuItem(5, "Colour Temperature", &item_3));
    item_3.addChild(MenuItem(6, "DMX", &item_3));
    
    MenuItem item_8 = MenuItem(7, "Effect", &root_menu_item);
    item_8.addChild(MenuItem(8, "Chase", &item_8));
    item_8.addChild(MenuItem(9, "Dim Up/Down", &item_8));
    item_8.addChild(MenuItem(10, "Flash", &item_8));
    item_8.addChild(MenuItem(11, "Strobe", &item_8));
    item_8.addChild(MenuItem(12, "Temperature Sensitive", &item_8));
    item_8.addChild(MenuItem(13, "Therapy", &item_8));

    MenuItem item_15 = MenuItem(14, "Color Temperature", &root_menu_item);
    item_15.addChild(MenuItem(15, "2700K", &item_15));
    item_15.addChild(MenuItem(16, "3000K", &item_15));
    item_15.addChild(MenuItem(17, "3500K", &item_15));
    item_15.addChild(MenuItem(18, "4000K", &item_15));
    item_15.addChild(MenuItem(19, "5000K", &item_15));
    item_15.addChild(MenuItem(20, "6000K", &item_15));
    item_15.addChild(MenuItem(21, "9000K", &item_15));
    
    root_menu_item.addChild(item_2);
    root_menu_item.addChild(item_3);
    root_menu_item.addChild(item_8);
    root_menu_item.addChild(item_15);

    menu_list = getList("", root_menu_item.children);
}

// Method to update the display buffer with the current menu items based on the current menu index
void MenuBuilder::updateMenu() {
    if (!menu_list.empty()) {
        display_buffer[0] = menu_list[current_menu_index];
        display_buffer[1] = (current_menu_index < menu_list.size() - 1) ? menu_list[current_menu_index + 1] : "";
    }
}

// Method to move the menu selection up
void MenuBuilder::moveMenuUp() {
    if (current_menu_index > 0) {
        current_menu_index--;
    }
}

// Method to move the menu selection down
void MenuBuilder::moveMenuDown() {
    if (current_menu_index < menu_list.size() - 1) {
        current_menu_index++;
    }
}

/*
std::vector<std::string> MenuBuilder::getList(const std::string& prefix, const std::vector<MenuItem>& children) {    
    if (children.empty()) return std::vector<std::string>();

    std::vector<std::string> list;
    for (const MenuItem& child : children) {
        list.push_back(prefix + child.label);
        auto child_list = getList(prefix + "-", child.children);
        list.insert(list.end(), child_list.begin(), child_list.end());
    }

    return list;
}
*/

// Static method to generate a flat list of menu items with prefixes to indicate their hierarchical level
std::vector<std::string> MenuBuilder::getList(const std::string& prefix, const std::vector<MenuItem>& children) {
    std::vector<std::string> list;
    if (children.empty()) return list;

    list.reserve(countNodes(children));

    std::string workingPrefix = prefix;
    buildList(workingPrefix, children, list);
    return list;
}