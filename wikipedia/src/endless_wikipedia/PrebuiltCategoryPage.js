const GObject = imports.gi.GObject;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;

const EndlessWikipedia = imports.endless_wikipedia.EndlessWikipedia;

GObject.ParamFlags.READWRITE = GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE;

const PrebuiltCategoryPage = new Lang.Class({
    Name: 'PrebuiltCategoryPage',
    Extends: Gtk.Frame,
    Properties: {
        'title': GObject.ParamSpec.string('title',
            'Name of category',
            'Name of the category to be displayed at the top of the category page',
            GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT,
            ''),
        'description': GObject.ParamSpec.string('description',
            'Description',
            'Description of the category (excerpt from Wiki text)',
            GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT,
            '')
    },

    _init: function(props) {
        this._title = null;
        this._description = null;

        this._grid = new Gtk.Grid({
            orientation: Gtk.Orientation.VERTICAL,
            expand: true
        });
        this._title_label = new Gtk.Label({
            vexpand: false,
            halign: Gtk.Align.START
        });
        this._separator = new Gtk.Separator({
            orientation: Gtk.Orientation.HORIZONTAL,
            vexpand: false
        });
        this._description_label = new Gtk.Label({
            valign: Gtk.Align.START,
            halign: Gtk.Align.START
        });

        this.parent(props);

        this._grid.add(this._title_label);
        this._grid.add(this._separator);
        this._grid.add(this._description_label);
        this.add(this._grid);

        let context = this._title_label.get_style_context();
        context.add_class(EndlessWikipedia.STYLE_CLASS_TITLE);
        context.add_class(EndlessWikipedia.STYLE_CLASS_CATEGORY);
        context.add_class(EndlessWikipedia.STYLE_CLASS_CATEGORY_PAGE);
        context.add_class(EndlessWikipedia.STYLE_CLASS_PREBUILT);
        let context = this._separator.get_style_context();
        context.add_class(EndlessWikipedia.STYLE_CLASS_CATEGORY_PAGE);
        let context = this._description_label.get_style_context();
        context.add_class(EndlessWikipedia.STYLE_CLASS_CATEGORY_PAGE);
        let context = this.get_style_context();
        context.add_class(EndlessWikipedia.STYLE_CLASS_CATEGORY_PAGE);
    },

    get title() {
        return this._title;
    },

    set title(value) {
        this._title = value;
        if(this._title_label)
            this._title_label.label = value.toUpperCase();
    },

    get description() {
        return this._description;
    },

    set description(value) {
        this._description = value;
        if(this._description_label)
            this._description_label.label = value;
    }
});