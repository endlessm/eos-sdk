const Endless = imports.gi.Endless;
const Lang = imports.lang;
const GObject = imports.gi.GObject;

//Local Libraries
const Utils = imports.wikipedia.utils;

const CategoryModel = imports.wikipedia.models.category_model;
const ArticleModel = imports.wikipedia.models.article_model;


function _resourceUriToPath(uri) {
    if(uri.startsWith('resource://'))
        return uri.slice('resource://'.length);
    throw new Error('Resource URI did not start with "resource://"');
}

function _pathnameToLanguage(uri) {
    let parts = uri.split("/");
    let filename = parts[parts.length-1];
    return filename.substring(0, 2);
}

const DomainWikiPresenter = new Lang.Class({
    Name: "DomainWikiPresenter",
    Extends: GObject.Object,

    _init: function(model, view, app_filename, linked_articles_filename) {
        this._model = model;
        this._view = view;
        this._view.set_presenter(this);
        this._view.connect('category-chosen',
            Lang.bind(this, this._onCategoryClicked));
        this._view.connect('article-chosen',
            Lang.bind(this, this._onArticleClicked));

        this.initAppInfoFromJsonFile(app_filename);
        this.initPageRankFromJsonFile(linked_articles_filename);

        let firstLevel = this._model.getMainCategory().getSubcategories();
        firstLevel.push(this._model.getMainCategory());
        this._view.set_categories(firstLevel);

        let linked_articles = this._model.getLinkedArticles();
        let to_show = linked_articles["app_articles"].concat(linked_articles["extra_linked_articles"]);
        this._view.set_showable_links(to_show);
    },

    initPageRankFromJsonFile: function(filename){
        let articles = JSON.parse(Utils.load_file_from_resource(filename));
        this._model.setLinkedArticles(articles);
    },

    initAppInfoFromJsonFile: function(filename) {
        let app_content = JSON.parse(Utils.load_file_from_resource(filename));
        this._model.loadFromJson(app_content);
    },

    // Respond to the front page's 'category-clicked' signal by loading the
    // articles belonging to that category and switching to the category page
    _onCategoryClicked: function (page, categoryId) {
        let newCategory = this._model.getCategory(categoryId);
        let articles = this._model.getArticlesForCategory(categoryId);

        this._view.set_category_info(newCategory, articles);

        this._view.transition_page(Endless.PageManagerTransitionType.SLIDE_LEFT,
            'category');
    },

    // Respond to the category page's 'article-clicked' signal by loading that
    // article and switching to the article page
    _onArticleClicked: function (articleList, title, uri) {
        this._view.set_article_info(title, uri);
        this._view.transition_page(Endless.PageManagerTransitionType.SLIDE_LEFT,
            'article');
    },

    _onCategoryBackClicked: function(button) {
        this._view.transition_page(
            Endless.PageManagerTransitionType.SLIDE_RIGHT, 'front');
    },

    _onArticleBackClicked: function(button) {
        this._view.transition_page(
            Endless.PageManagerTransitionType.SLIDE_RIGHT, 'category');
    }
});