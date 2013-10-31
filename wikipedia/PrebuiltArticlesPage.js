const GObject = imports.gi.GObject;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;

const EndlessWikipedia = imports.wikipedia.EndlessWikipedia;

GObject.ParamFlags.READWRITE = GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE;

const PrebuiltArticlesPage = new Lang.Class({
    Name: 'PrebuiltArticlesPage',
    Extends: Gtk.Frame,
    Properties: {
        'article-title': GObject.ParamSpec.string('article-title',
            'Article title',
            'Human-readable title for the article to be displayed',
            GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT,
            ''),
        'article-uri': GObject.ParamSpec.string('article-uri',
            'Article URI',
            'Wikipedia URI for the article to be displayed',
            GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT,
            '')
    },

    _init: function(props) {
        this._article_title = null;
        this._article_uri = null;

        // Empty array is placeholder until we get baby page rank
        this._wiki_view = new EndlessWikipedia.WikipediaWebView({
            expand: true,
            hide_links: true
        });

        this.parent(props);

        this.add(this._wiki_view);

        // Add style contexts for CSS
        let context = this.get_style_context();
        context.add_class(EndlessWikipedia.STYLE_CLASS_ARTICLES_PAGE);
    },

    setShowableLinks: function(linked_articles) {
        this._wiki_view.setShowableLinks(linked_articles);
    },

    set_app_name: function (app_name) {
        this._wiki_view.app_name = app_name;
    },

    set_personality: function (personality) {
        this._wiki_view.system_personality = personality;
    },

    get article_title() {
        return this._article_title;
    },

    set article_title(value) {
        this._article_title = value;
    },

    get article_uri() {
        return this._article_uri;
    },

    set article_uri(value) {
        this._article_uri = value;
        if(value !== null && value !== "") {
            this._wiki_view.loadArticleByTitle(this._article_title);
        }
    }
});