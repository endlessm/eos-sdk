const GObject = imports.gi.GObject;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;

const EndlessWikipedia = imports.wikipedia.EndlessWikipedia;
const WikipediaView = imports.views.wikipedia_view;

GObject.ParamFlags.READWRITE = GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE;

const PrebuiltArticlesPage = new Lang.Class({
    Name: 'PrebuiltArticlesPage',
    Extends: Gtk.Frame,
    Properties: {
        'article-title': GObject.ParamSpec.string('article-title',
            'Article title',
            'Human-readable title for the article to be displayed',
            GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT,
            '')
    },

    _init: function(props) {
        this._article_title = null;

        this._wiki_view = new WikipediaView.WikipediaView({
            expand:true,
        });

        this.parent(props);

        this.add(this._wiki_view);

        // Add style contexts for CSS
        let context = this.get_style_context();
        context.add_class(EndlessWikipedia.STYLE_CLASS_ARTICLES_PAGE);
    },

    get article_title() {
        return this._article_title;
    },

    set article_title(value) {
        this._article_title = value;
        if(value !== null && value !== "") {
            this._wiki_view.loadArticleByTitle(this._article_title);
        }
    }
});