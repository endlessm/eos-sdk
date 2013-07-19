const GObject = imports.gi.GObject;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;

const EndlessWikipedia = imports.endless_wikipedia.EndlessWikipedia;

GObject.ParamFlags.READWRITE = GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE;

const PrebuiltFrontPage = new Lang.Class({
    Name: 'PrebuiltFrontPage',
    Extends: Gtk.Grid,
    Properties: {
        'title': GObject.ParamSpec.string('title',
            'Front page title',
            'Name of the Wikipedia-based application',
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

        this._title_label = new Gtk.Label({
            expand: true,
            halign: Gtk.Align.START,
            valign: Gtk.Align.END
        });
        let context = this._title_label.get_style_context()
        context.add_class(EndlessWikipedia.STYLE_CLASS_TITLE);
        context.add_class(EndlessWikipedia.STYLE_CLASS_PREBUILT);
        context.add_class(EndlessWikipedia.STYLE_CLASS_FRONT_PAGE);
        this._category_selector = new EndlessWikipedia.CategorySelector();

        props = props || {};
        props.orientation = Gtk.Orientation.VERTICAL;
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
        if(this._title_label)
            this._title_label.label = value.toUpperCase();
    },

    setCategories: function(categories) {
        this._category_selector.setCategories(categories);
    },

    // Proxy signal
    _onCategoryChosen: function(widget, title, index) {
        this.emit('category-chosen', title, index);
    }
});