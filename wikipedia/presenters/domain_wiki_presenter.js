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
        this._domain_wiki_model = model;
        this._domain_wiki_view = view;
        this._domain_wiki_view.set_presenter(this)
        this._domain_wiki_view.connect('category-chosen', Lang.bind(this, this._onCategoryClicked));
        this._domain_wiki_view.connect('article-chosen', Lang.bind(this, this._onArticleClicked));

        this.initAppInfoFromJsonFile(app_filename);
        this.initPageRankFromJsonFile(linked_articles_filename);

        this._domain_wiki_view.set_categories(this._domain_wiki_model.getCategories());

        let linked_articles = this._domain_wiki_model.getLinkedArticles();
        let to_show = linked_articles["app_articles"].concat(linked_articles["extra_linked_articles"]);
        this._domain_wiki_view.set_showable_links(to_show);
    },

    initArticleModels: function(articles) {
        let _articles = new Array();
        for(let i = 0; i < articles.length; i++) {
            let humanTitle = articles[i].title;
            let wikipediaURL = articles[i].url;
            let newArticle = new ArticleModel.ArticleModel({ title: humanTitle, uri: wikipediaURL});
            _articles.push(newArticle);
        }
      return _articles;
    },

    initPageRankFromJsonFile: function(filename){
        let articles = JSON.parse(Utils.load_file_from_resource(filename));
        this._domain_wiki_model.setLinkedArticles(articles);
    },

    initAppInfoFromJsonFile: function(filename) {
        let app_content = JSON.parse(Utils.load_file_from_resource(filename));
        this._domain_wiki_view.set_lang(_pathnameToLanguage(filename));
        let categories = app_content['categories'];
        let cat_length = categories.length
        let category_models = new Array();
        for(let i = 0; i < cat_length; i++){
            let category = categories[i];
            let categoryModel = this.initCategory(category);
            let articles = category['articles'];
            let articleModels = [];
            if(!(articles.length == 0)) {
                //if the category has no articles, then we cannot initialize them.
                //This happens if the main category isn't clickable.
                articleModels = this.initArticleModels(articles);
            }
            categoryModel.addArticles(articleModels);
            category_models.push(categoryModel);
        }
        this._domain_wiki_model.addCategories(category_models);
    },

    initCategory: function(category){
        let image_uri = category['image_file'];
        let image_thumbnail_uri = category['image_thumb_uri'];
        let params = {description:category['content_text'], image_uri:image_uri, 
            image_thumbnail_uri:image_thumbnail_uri, title:category['category_name'], 
            is_main_category:category['is_main_category']};
        return new CategoryModel.CategoryModel(params);
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