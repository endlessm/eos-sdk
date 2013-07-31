const GObject = imports.gi.GObject;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;

const EndlessWikipedia = imports.endless_wikipedia.EndlessWikipedia;
const WikipediaView = imports.views.wikipedia_view;

GObject.ParamFlags.READWRITE = GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE;

const PrebuiltArticlesPage = new Lang.Class({
    Name: 'PrebuiltArticlesPage',
    Extends: Gtk.Frame,
    Properties: {
        'title': GObject.ParamSpec.string('title',
            'Article title',
            'Human-readable title for the article to be displayed',
            GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT,
            ''),
        'article-uri': GObject.ParamSpec.string('article-uri',
            'Article URI',
            'Last component of the Wikipedia URI for the article',
            GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT,
            '')
    },

    _init: function(props) {
        this._title = null;
        //We don't use article_uri now since the database only needs to readable title.
        this._article_uri = null;

        // Create widgets
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
        this._wiki_view = new WikipediaView.WikipediaView();
        // Put the webview in a scrolledWindow to handle large page sizes
        let scrolledWindow = new Gtk.ScrolledWindow ({
            hscrollbar_policy: Gtk.PolicyType.AUTOMATIC,
            vscrollbar_policy: Gtk.PolicyType.AUTOMATIC,
            expand: true});
        scrolledWindow.add(this._wiki_view);

        this.parent(props);

        this._grid.add(this._title_label);
        this._grid.add(this._separator);
        this._grid.add(scrolledWindow);
        this.add(this._grid);

        // Add style contexts for CSS
        let context = this._title_label.get_style_context();
        context.add_class(EndlessWikipedia.STYLE_CLASS_TITLE);
        context.add_class(EndlessWikipedia.STYLE_CLASS_ARTICLE);
        context.add_class(EndlessWikipedia.STYLE_CLASS_ARTICLES_PAGE);
        context.add_class(EndlessWikipedia.STYLE_CLASS_PREBUILT);
        let context = this._separator.get_style_context();
        context.add_class(EndlessWikipedia.STYLE_CLASS_ARTICLES_PAGE);
        let context = this.get_style_context();
        context.add_class(EndlessWikipedia.STYLE_CLASS_ARTICLES_PAGE);
    },

    get title() {
        return this._title;
    },

    set title(value) {
        this._title = value;
        if(this._title_label)
            this._title_label.label = value.toUpperCase();
    },

    get article_uri() {
        return this._article_uri;
    },

    set article_uri(value) {
        this._article_uri = value;
    }
});