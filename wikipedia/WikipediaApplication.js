const Endless = imports.gi.Endless;
const Gdk = imports.gi.Gdk;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;
const GObject = imports.gi.GObject;
const Gio = imports.gi.Gio;

const Config = imports.wikipedia.config;
const DomainWikiModel = imports.wikipedia.models.domain_wiki_model;

GObject.ParamFlags.READWRITE = GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE;

const WikipediaApplication = new Lang.Class({
    Name: 'WikipediaApplication',
    Extends: Endless.Application,
    Properties: {
        // resource:// URI for the categories JSON file
        'application-uri': GObject.ParamSpec.string('application-uri',
            'Category file URI',
            'URI for the data file describing the categories and articles',
            GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT_ONLY,
            ''),

        // Name of the Wikipedia-based application, e.g. 'Brazil', 'Math'
        'application-name': GObject.ParamSpec.string('application-name',
            'Application name',
            'Name of the Wikipedia-based application',
            GObject.ParamFlags.READABLE,
            ''),

        // Name of the Wikipedia-based application, e.g. 'Brazil', 'Math'
        'application-base-path': GObject.ParamSpec.string('application-base-path',
            'Application Base Path',
            'Path to base directory where execution began',
            GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT_ONLY,
            '')
    },

    _init: function(props) {
        this.parent(props);
    },

    // VIRTUAL FUNCTIONS

    vfunc_startup: function() {
        // Load GResource bundle
        let resource = Gio.Resource.load(Config.WIKIPEDIA_DATADIR + 'eos-wikipedia-domain.gresource');
        resource._register();

        this.parent();
        this._domain_wiki_model = new DomainWikiModel.DomainWikiModel();

        let provider = new Gtk.CssProvider();
        let css_file = Gio.File.new_for_uri('resource:///com/endlessm/wikipedia-domain/css/eos-wikipedia-domain.css')
        provider.load_from_file(css_file);
        Gtk.StyleContext.add_provider_for_screen(Gdk.Screen.get_default(), provider, Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION)
    }

});
