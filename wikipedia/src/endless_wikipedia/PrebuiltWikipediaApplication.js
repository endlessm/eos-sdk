const Endless = imports.gi.Endless;
const Format = imports.format;
const Lang = imports.lang;
const Gtk = imports.gi.Gtk;

const EndlessWikipedia = imports.endless_wikipedia.EndlessWikipedia;
const DomainWikiView = imports.views.domain_wiki_view;
const DomainWikiModel = imports.models.domain_wiki_model;
const DomainWikiPresenter = imports.presenters.domain_wiki_presenter;

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
        this._domain_wiki_view = new DomainWikiView.DomainWikiView(this);
        let filename = this.application_uri;
        this._domain_wiki_presenter = new DomainWikiPresenter.DomainWikiPresenter(this._domain_wiki_model, this._domain_wiki_view, filename);

        this._domain_wiki_view.set_categories(categories);

        this._domain_wiki_view.connect('category-chosen', Lang.bind(this, this._onCategoryClicked));
        this._domain_wiki_view.connect('article-chosen', Lang.bind(this, this._onArticleClicked));

        //this._view = new WikipediaView.WikipediaView(this);
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

        this._domain_wiki_view.transition_page(Endless.PageManagerTransitionType.SLIDE_LEFT, 'category');

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
