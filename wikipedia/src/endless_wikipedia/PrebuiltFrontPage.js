const GObject = imports.gi.GObject;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;

const EndlessWikipedia = imports.endless_wikipedia.EndlessWikipedia;
const CategorySelectorView = imports.views.category_selector_view;
const TitleLabelView = imports.views.title_label_view;

const TITLE_CATEGORY_COLUMN_SPACING = 10;  // pixels

GObject.ParamFlags.READWRITE = GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE;

const PrebuiltFrontPage = new Lang.Class({
    Name: 'PrebuiltFrontPage',
    Extends: Gtk.Grid,
    Properties: {
        'title': GObject.ParamSpec.string('title',
            'Front page title',
            'Name of the Wikipedia-based application',
            GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT,
            ''),
        'image-uri': GObject.ParamSpec.string('image-uri',
            'Image URI',
            'Image URI for title image',
            GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT,
            '')
    },
    Signals: {
        'category-chosen': {
            param_types: [GObject.TYPE_STRING, GObject.TYPE_INT]
        }
    },

    _init: function(props) {
        this._title = null;
        this._image_uri = null;

        this._title_label = new TitleLabelView.TitleLabelView();
        let context = this._title_label.get_style_context()
        context.add_class(EndlessWikipedia.STYLE_CLASS_TITLE);
        context.add_class(EndlessWikipedia.STYLE_CLASS_PREBUILT);
        context.add_class(EndlessWikipedia.STYLE_CLASS_FRONT_PAGE);
        this._category_selector = new CategorySelectorView.CategorySelectorView();

        props = props || {};
        props.orientation = Gtk.Orientation.HORIZONTAL;
        props.column_spacing = TITLE_CATEGORY_COLUMN_SPACING;
        this.parent(props);

        this.add(this._title_label);
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
    _onCategoryChosen: function(widget, title, index) {
        this.emit('category-chosen', title, index);
    }
});