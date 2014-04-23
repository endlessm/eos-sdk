const Endless = imports.gi.Endless;
const Gio = imports.gi.Gio;
const GObject = imports.gi.GObject;
const Lang = imports.lang;
const System = imports.system;

//Local Libraries
const Utils = imports.wikipedia.utils;

const CategoryModel = imports.wikipedia.models.category_model;
const ArticleModel = imports.wikipedia.models.article_model;


function _resourceUriToPath(uri) {
    if(uri.startsWith('resource://'))
        return uri.slice('resource://'.length);
    throw new Error('Resource URI did not start with "resource://"');
}

function _pathnameToAppName(uri) {
    let parts = uri.split("/");
    let filename = parts[parts.length-1];
    // Split by both dashes and periods in order
    // to retrieve, e.g. 'health' from 'health-Guatemala.json'
    let filename_parts = filename.split(/[\-\.]/);
    return filename_parts[0];
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
        this._view.connect('category-back-clicked',
            Lang.bind(this, this._onCategoryBackClicked));
        this._view.connect('article-back-clicked',
            Lang.bind(this, this._onArticleBackClicked));

        this.initAppInfoFromJsonFile(app_filename);

        if(linked_articles_filename !== '')
            this.initPageRankFromJsonFile(linked_articles_filename);

        let firstLevel = this._model.getMainCategory().getSubcategories();
        firstLevel.push(this._model.getMainCategory());
        this._view.set_categories(firstLevel);

        let to_show = this._model.getLinkedArticles();
        this._view.set_showable_links(to_show);

        let app_name = _pathnameToAppName(app_filename);
        let personality = Endless.get_system_personality();

        this._view.set_personality(personality);
        this._view.set_app_name(app_name);
        this.parent();
    },

    initPageRankFromJsonFile: function(filename){
        let articles = JSON.parse(Utils.load_file_from_resource(filename));
        this._model.setLinkedArticles(articles);
    },

    initAppInfoFromJsonFile: function(filename) {
        try {
            let app_content = JSON.parse(Utils.load_file_from_resource(filename));
            this._model.loadFromJson(app_content);
        } catch(e) {
            print(e);
            if (e.matches(Gio.IOErrorEnum, Gio.IOErrorEnum.NOT_FOUND)) {
                print("****** This app does not support the personality",
                      Endless.get_system_personality(), "******");
            }
            System.exit(1);
        }
    },

    // Respond to the front page's 'category-clicked' signal by loading the
    // articles belonging to that category and switching to the category page
    _onCategoryClicked: function (page, categoryId) {
        let newCategory = this._model.getCategory(categoryId);
        let articles = this._model.getArticlesForCategory(categoryId);

        this._view.set_category_info(newCategory, articles);

        this._view.show_category_page();
    },

    // Respond to the category page's 'article-clicked' signal by loading that
    // article and switching to the article page
    _onArticleClicked: function (articleList, title, uri) {
        this._view.set_article_info(title, uri);
        this._view.show_article_page();
    },

    _onCategoryBackClicked: function(button) {
        this._view.show_front_page();
    },

    _onArticleBackClicked: function(button) {
        this._view.show_category_page();
    }
});
