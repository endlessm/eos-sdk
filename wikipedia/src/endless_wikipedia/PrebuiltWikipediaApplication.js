const Endless = imports.gi.Endless;
const Format = imports.format;
const Lang = imports.lang;
const Gtk = imports.gi.Gtk;

const EndlessWikipedia = imports.endless_wikipedia.EndlessWikipedia;
const DomainWikiView = imports.views.domain_wiki_view;
const DomainWikiModel = imports.models.domain_wiki_model;
//const DomainWikiPresenter = imports.presenters.domain_wiki_presenter;

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
        let categories = this._domain_wiki_model.getCategories();
        this._domain_wiki_view = new DomainWikiView.DomainWikiView(this);


        this._domain_wiki_view.set_categories(categories);

        this._domain_wiki_view.connect('category-chosen', Lang.bind(this, this._onCategoryClicked));
        this._domain_wiki_view.connect('article-chosen', Lang.bind(this, this._onArticleClicked));

        //this._domain_wiki_model = new DomainWikiModel.DomainWikiModel();
        //this._domain_wiki_presenter = new DomainPresenter.DomainWikiPresenter(this._wiki_model, this._wiki_view);

        this._view = new WikipediaView.WikipediaView(this);

        //this._presenter = new WikipediaPresenter.WikipediaPresenter(this._model, this._view);


        // Front page
        // this._front_page = new EndlessWikipedia.PrebuiltFrontPage({
        //     title: this._model.application_name
        // });
        // this._front_page.setCategories(this._model.getCategories());
        // this._front_page.connect('category-chosen',
        //     Lang.bind(this, this._onCategoryClicked));

        // // Category page
        // this._category_page = new Gtk.Grid({
        //     orientation: Gtk.Orientation.HORIZONTAL
        // });
        // this._category_view = new EndlessWikipedia.PrebuiltCategoryPage();
        // // _category_article_list is eventually going to be the same widget as
        // // _article_list, so that's why it's not built into the
        // // PrebuiltCategoryPage
        // this._category_article_list = new EndlessWikipedia.ArticleList({
        //     halign: Gtk.Align.END,
        //     hexpand: false
        // });
        // this._category_page.add(this._category_view);
        // this._category_page.add(this._category_article_list);

        // this._category_back_button = new EndlessWikipedia.BackButton({
        //     label: _('START')
        // });
        // this._category_back_button.show();

        // this._category_article_list.connect('article-chosen',
        //     Lang.bind(this, this._onArticleClicked));
        // this._category_back_button.connect('clicked',
        //     Lang.bind(this, this._onCategoryBackClicked));

        // // Article page
        // this._article_page = new Gtk.Grid({
        //     orientation: Gtk.Orientation.HORIZONTAL
        // });
        // this._article_view = new EndlessWikipedia.PrebuiltArticlesPage();
        // this._article_list = new EndlessWikipedia.ArticleList({
        //     halign: Gtk.Align.START,
        //     hexpand: false
        // });
        // this._article_page.add(this._article_list);
        // this._article_page.add(this._article_view);

        // this._article_back_button = new EndlessWikipedia.BackButton();
        // this._article_back_button.show();

        // this._article_list.connect('article-chosen',
        //     Lang.bind(this, this._onArticleClicked));
        // this._article_back_button.connect('clicked',
        //     Lang.bind(this, this._onArticleBackClicked));

        // // Build window
        // this._window = new Endless.Window({
        //     title: this._model.application_name,
        //     application: this
        // });
        // this._window.page_manager.transition_duration = 200;  // ms
        // this._window.page_manager.add(this._front_page, {
        //     name: 'front'
        // });
        // this._window.page_manager.add(this._category_page, {
        //     name: 'category',
        //     left_topbar_widget: this._category_back_button
        // });
        // this._window.page_manager.add(this._article_page, {
        //     name: 'article',
        //     left_topbar_widget: this._article_back_button
        // })
        // this._window.show_all();
    },

    _onCategoryClicked: function(page, title, index) {
        this._current_category = index;
        let category = this._domain_wiki_model.getCategories()[index];
        let articles = this._domain_wiki_model.getArticlesForCategoryIndex(index);

        let titles = new Array();
        for(let i = 0; i < articles.length; i++){
            titles.push(articles[i].title);
        }

        this._domain_wiki_view.set_category_info(category, titles);


        //this._domain_wiki_view.set_category_title(category.title);
        //this._domain_wiki_view.set_category_description(category.content);

        //this._category_view.title = category.title;
        //this._category_view.description = category.content;

        //this._category_article_list.setArticles(articles);
        //this._article_list.setArticles(articles);
        //this._article_back_button.label = category.title.toUpperCase();

        this._domain_wiki_view.transition_page(Endless.PageManagerTransitionType.SLIDE_LEFT, 'category');

        //this._window.page_manager.transition_type = Endless.PageManagerTransitionType.SLIDE_LEFT;
        //this._window.page_manager.visible_page_name = 'category';
    },

    _onArticleClicked: function(article_list, title, index) {
        // Need to know which category this came from!!
        //this._article_view.title = title;
        //this._article_view.article_uri = uri;
        let articles = this._domain_wiki_model.getArticlesForCategoryIndex(this._current_category);
        this._domain_wiki_view.set_article_info(articles[index]);
        this._domain_wiki_view.transition_page(Endless.PageManagerTransitionType.SLIDE_LEFT, 'article');

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
