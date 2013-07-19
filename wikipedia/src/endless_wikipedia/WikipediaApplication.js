const Lang = imports.lang;
const GObject = imports.gi.GObject;
const Gio = imports.gi.Gio;

GObject.ParamFlags.READWRITE = GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE;

const WikipediaApplication = new Lang.Class({
    Name: 'WikipediaApplication',
    Extends: Endless.Application,
    Properties: {
        // resource:// URI for the categories JSON file
        'categories-uri': GObject.ParamSpec.string('categories-uri',
            'Category file URI',
            'URI for the data file describing the categories and articles',
            GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT_ONLY,
            ''),

        // Name of the Wikipedia-based application, e.g. 'Brazil', 'Math'
        'application-name': GObject.ParamSpec.string('application-name',
            'Application name',
            'Name of the Wikipedia-based application',
            GObject.ParamFlags.READABLE,
            '')
    },

    _init: function(props) {
        this._categories_uri = null;
        this._categories = null;
        this.parent(props);
    },

    // PROPERTIES

    get categories_uri() {
        return this._categories_uri;
    },

    set categories_uri(value) {
        this._categories_uri = value;
    },

    get application_name() {
        if(this._categories)
            return this._categories.app_name;
        return undefined;
    },

    // VIRTUAL FUNCTIONS

    vfunc_startup: function() {
        this.parent();

        let category_file = Gio.File.new_for_uri(this.categories_uri);
        let [success, category_json, etag] = category_file.load_contents(null);
        this._categories = JSON.parse(category_json);

        // Doesn't belong here
        let provider = new Gtk.CssProvider();
        let css_file = Gio.File.new_for_uri('resource:///com/endlessm/brazil/css/endless_brazil.css')
        provider.load_from_file(css_file);
        Gtk.StyleContext.add_provider_for_screen(Gdk.Screen.get_default(), provider, Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION);
    },

    // PUBLIC API

    getCategories: function() {
        let category_list = this._categories.categories.map(function(element, index, obj) {
            return {
                title: element.category_name,
                image_uri: element.image_uri,
                content: element.content_text
            };
        })
        return category_list;
    },

    getArticlesForCategory: function(category_title) {
        //...
    },

    getArticlesForCategoryIndex: function(index) {
        return this._categories.categories[index].articles;
    }

});
