const Endless = imports.gi.Endless;
const GObject = imports.gi.GObject;
const Lang = imports.lang;

// Local libraries
const ArticleModel = imports.models.article_model;

const CategoryModel = new Lang.Class({
    Name: "CategoryModel",
    Extends: GObject.Object,
   	Properties: {
<<<<<<< HEAD
        'description': GObject.ParamSpec.string('description', 'Category Description', 'This is the text that the user reads on the category page.',
            GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE | GObject.ParamFlags.CONSTRUCT,
                ""),
        'title': GObject.ParamSpec.string('title', 'Category Name', 'This is the name that is displayed on the front page and as the title on the category page.', 
            GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE | GObject.ParamFlags.CONSTRUCT,
                ""),
        'image_uri': GObject.ParamSpec.string('image_uri', 'Category Image URI', 'Path to image for this category in the GResource', 
            GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE | GObject.ParamFlags.CONSTRUCT,
                "")
    },

    _init: function(params) {
        this.parent(params);
    },

    addArticles: function(articles) {
        this._articles = articles;
    }
=======
        'description': GObject.ParamSpec.string('description', 'Category Description', 'Category Description',
                                       GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE | GObject.ParamFlags.CONSTRUCT,
                                       ""),
        'title': GObject.ParamSpec.string('title', 'Category Name', 'Category Name', 
                                         GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE | GObject.ParamFlags.CONSTRUCT,
                                         ""),
        'image_uri': GObject.ParamSpec.string('image_uri', 'Category Image URI', 'Category Image URI', 
                                          GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE | GObject.ParamFlags.CONSTRUCT,
                                          "")
    },


    _init: function(category_title, desc, image_uri, articles) {
        let params = {};
        params["title"] = category_title;
        params["description"] = desc;
        params["image_uri"] = image_uri;
        this.parent(params);
        this._articles = this.createArticleModels(articles);
    },

    createArticleModels: function(articles) {
    	let _articles = new Array();
    	for(let i = 0; i < articles.length; i++) {
    		let humanTitle = articles[i].title;
    		let wikipediaURL = articles[i].url;

    		let newArticle = new ArticleModel.ArticleModel({ title: humanTitle, uri: wikipediaURL});
    		_articles.push(newArticle);

    	}
      return _articles;

    },
>>>>>>> 9a85677... Finished category and application model

    getArticles: function() {
        return this._articles;
    },
});