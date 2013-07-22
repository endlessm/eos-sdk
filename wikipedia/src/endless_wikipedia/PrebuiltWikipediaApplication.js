const Endless = imports.gi.Endless;
const Format = imports.format;
const Lang = imports.lang;
const Gtk = imports.gi.Gtk;

const EndlessWikipedia = imports.endless_wikipedia.EndlessWikipedia;
const WikipediaModel = imports.models.WikipediaModel;

const _ = function(x) { return x; };

String.prototype.format = Format.format;

const PrebuiltWikipediaApplication = new Lang.Class({
    Name: 'PrebuiltWikipediaApplication',
    Extends: EndlessWikipedia.WikipediaApplication,

    _init: function(props) {
        this.parent(props);
    },

    vfunc_startup: function() {
        this.parent();


        //this._model = new WikipediaModel.WikipediaModel("pt-BR-brazil.json");
        //this._view = new WikipediaView.WikipediaView(this);

        //this._presenter = new WikipediaPresenter.WikipediaPresenter(this._model, this._view);


        // Front page
        this._front_page = new EndlessWikipedia.PrebuiltFrontPage({
            title: this._model.application_name
        });
        this._front_page.setCategories(this._model.getCategories());
        this._front_page.connect('category-chosen',
            Lang.bind(this, this._onCategoryClicked));

        // Category page
        this._category_page = new Gtk.Grid({
            orientation: Gtk.Orientation.HORIZONTAL
        });
        this._category_view = new EndlessWikipedia.PrebuiltCategoryPage();
        // _category_article_list is eventually going to be the same widget as
        // _article_list, so that's why it's not built into the
        // PrebuiltCategoryPage
        this._category_article_list = new EndlessWikipedia.ArticleList({
            halign: Gtk.Align.END,
            hexpand: false
        });
        this._category_page.add(this._category_view);
        this._category_page.add(this._category_article_list);

        this._category_back_button = new EndlessWikipedia.BackButton({
            label: _('START')
        });
        this._category_back_button.show();

        this._category_article_list.connect('article-chosen',
            Lang.bind(this, this._onArticleClicked));
        this._category_back_button.connect('clicked',
            Lang.bind(this, this._onCategoryBackClicked));

        // Article page
        this._article_page = new Gtk.Grid({
            orientation: Gtk.Orientation.HORIZONTAL
        });
        this._article_view = new EndlessWikipedia.PrebuiltArticlesPage();
        this._article_list = new EndlessWikipedia.ArticleList({
            halign: Gtk.Align.START,
            hexpand: false
        });
        this._article_page.add(this._article_list);
        this._article_page.add(this._article_view);

        this._article_back_button = new EndlessWikipedia.BackButton();
        this._article_back_button.show();

        this._article_list.connect('article-chosen',
            Lang.bind(this, this._onArticleClicked));
        this._article_back_button.connect('clicked',
            Lang.bind(this, this._onArticleBackClicked));

        // Build window
        this._window = new Endless.Window({
            title: this._model.application_name,
            application: this
        });
        this._window.page_manager.transition_duration = 200;  // ms
        this._window.page_manager.add(this._front_page, {
            name: 'front'
        });
        this._window.page_manager.add(this._category_page, {
            name: 'category',
            left_topbar_widget: this._category_back_button
        });
        this._window.page_manager.add(this._article_page, {
            name: 'article',
            left_topbar_widget: this._article_back_button
        })
        this._window.show_all();
    },

    _onCategoryClicked: function(page, title, index) {
        let category = this.getCategories()[index];
        let articles = this.getArticlesForCategoryIndex(index);

        this._category_view.title = category.title;
        this._category_view.description = category.content;
        this._category_article_list.setArticles(articles);
        this._article_list.setArticles(articles);
        this._article_back_button.label = category.title.toUpperCase();

        this._window.page_manager.transition_type = Endless.PageManagerTransitionType.SLIDE_LEFT;
        this._window.page_manager.visible_page_name = 'category';
    },

    _onArticleClicked: function(article_list, title, uri) {
        this._article_view.title = title;
        this._article_view.article_uri = uri;

        this._window.page_manager.transition_type = Endless.PageManagerTransitionType.SLIDE_LEFT;
        this._window.page_manager.visible_page_name = 'article'; 
    },

    _onCategoryBackClicked: function(button) {
        this._window.page_manager.transition_type = Endless.PageManagerTransitionType.SLIDE_RIGHT;
        this._window.page_manager.visible_page_name = 'front';
    },

    _onArticleBackClicked: function(button) {
        this._window.page_manager.transition_type = Endless.PageManagerTransitionType.SLIDE_RIGHT;
        this._window.page_manager.visible_page_name = 'category';
    }
});
