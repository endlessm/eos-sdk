const GObject = imports.gi.GObject;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;

const EndlessWikipedia = imports.wikipedia.EndlessWikipedia;
const CategorySelectorView = imports.wikipedia.widgets.category_selector_view;

const TITLE_CATEGORY_COLUMN_SPACING = 10;  // pixels

GObject.ParamFlags.READWRITE = GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE;

const PrebuiltFrontPage = new Lang.Class({
    Name: 'PrebuiltFrontPage',
    Extends: Gtk.Grid,
    Signals: {
        'category-chosen': {
            param_types: [GObject.TYPE_STRING]
        }
    },

    _init: function(props) {

        this._category_selector = new CategorySelectorView.CategorySelectorView();

        props = props || {};
        props.orientation = Gtk.Orientation.HORIZONTAL;
        props.column_spacing = TITLE_CATEGORY_COLUMN_SPACING;
        this.parent(props);

        this.add(this._category_selector);
        this._category_selector.connect('category-chosen',
            Lang.bind(this, this._onCategoryChosen));
    },

    get title() {
        return this._title;
    },

    set title(value) {
        this._title = value;
        this._title_label.title = value;
    },

    get image_uri() {
        return this._image_uri;
    },

    set image_uri(value) {
        this._image_uri = value;
        this._title_label.image_uri = value;
    },

    setCategories: function(categories) {
        this._category_selector.setCategories(categories);
    },

    // Proxy signal
    _onCategoryChosen: function(widget, categoryId) {
        this.emit('category-chosen', categoryId);
    }
});