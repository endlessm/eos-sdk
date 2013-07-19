const GObject = imports.gi.GObject;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;

const CategoryButton = imports.endless_wikipedia.CategoryButton;

const CategorySelector = new Lang.Class({
    Name: 'CategorySelector',
    Extends: Gtk.Grid,
    Signals: {
        'category-chosen': {
            param_types: [GObject.TYPE_STRING, GObject.TYPE_INT]
        }
    },

    _init: function(props) {
        props = props || {};
        props.row_homogeneous = true;
        props.column_homogeneous = true;
        this.parent(props);
    },

    // Takes an array of dictionaries with keys 'title' and 'image_uri'
    setCategories: function(categories) {
        categories.forEach(function(category, index, obj) {
            let button = new CategoryButton.CategoryButton({
                category_title: category.title,
                image_uri: category.image_uri
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