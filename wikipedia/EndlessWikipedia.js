const Endless = imports.gi.Endless;

// Pull modules into this namespace, sort of like __init__.py and __all__
this.WikipediaApplication = imports.wikipedia.WikipediaApplication.WikipediaApplication;
this.PrebuiltWikipediaApplication = imports.wikipedia.PrebuiltWikipediaApplication.PrebuiltWikipediaApplication;
this.PrebuiltFrontPage = imports.wikipedia.PrebuiltFrontPage.PrebuiltFrontPage;
this.PrebuiltCategoryPage = imports.wikipedia.PrebuiltCategoryPage.PrebuiltCategoryPage;
this.PrebuiltArticlesPage = imports.wikipedia.PrebuiltArticlesPage.PrebuiltArticlesPage;
this.ArticleList = imports.wikipedia.ArticleList.ArticleList;
this.WikipediaWebView = imports.wikipedia.WikipediaWebView.WikipediaWebView;

const STYLE_CLASS_TITLE = 'title';
const STYLE_CLASS_PREBUILT = 'prebuilt';
const STYLE_CLASS_CATEGORY = 'category';
const STYLE_CLASS_ARTICLE = 'article';
const STYLE_CLASS_FRONT_PAGE = 'front-page';
const STYLE_CLASS_CATEGORY_PAGE = 'category-page';
const STYLE_CLASS_ARTICLES_PAGE = 'articles-page';
