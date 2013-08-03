const GObject = imports.gi.GObject;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;

const CategoryButton = imports.views.category_button;
const CategoryLayoutManager = imports.views.category_layout_manager;

const CATEGORY_COLUMN_SPACING = 10;  // pixels
const CATEGORY_ROW_SPACING = 10;  // pixels

const CategorySelectorView = new Lang.Class({
    Name: 'CategorySelectorView',
    Extends: CategoryLayoutManager.CategoryLayoutManager,
    Signals: {
        'category-chosen': {
            param_types: [GObject.TYPE_STRING, GObject.TYPE_INT]
        }
    },

    _init: function(props) {
        props = props || {};
        props.column_spacing = CATEGORY_COLUMN_SPACING;
        props.row_spacing = CATEGORY_ROW_SPACING;
        this.parent(props);
    },

    // Takes an array of dictionaries with keys 'title' and 'image_uri'
    setCategories: function(categories) {
        categories.forEach(function(category, index, obj) {
            let button = new CategoryButton.CategoryButton({
                category_title: category.title,
                image_uri: category.image_thumbnail_uri
            });
            button.index = index;
            button.connect('clicked', Lang.bind(this, this._onButtonClicked));
            this.add(button);
        }, this);
    },

    _onButtonClicked: function(button) {
        this.emit('category-chosen', button.category_title, button.index);
    }
});