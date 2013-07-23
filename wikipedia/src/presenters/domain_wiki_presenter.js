const Lang = imports.lang;
const GObject = imports.gi.GObject;

const DomainWikiPresenter = new Lang.Class({
    Name: "DomainWikiPresenter",
    Extends: GObject.Object,

    _init: function(model, view) {
        this._domain_wiki_model = model;
        this._domain_wiki_view = view;
        this._domain_wiki_view.set_presenter(this)
        this._domain_wiki_view.connect('category-chosen', Lang.bind(this, this._onCategoryClicked));
        this._domain_wiki_view.connect('article-chosen', Lang.bind(this, this._onArticleClicked));

        let categories = this._domain_wiki_model.getCategories();

        this._domain_wiki_view.set_categories(categories);
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