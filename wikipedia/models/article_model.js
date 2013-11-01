const Endless = imports.gi.Endless;
const GObject = imports.gi.GObject;
const Lang = imports.lang;

GObject.ParamFlags.READWRITE = GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE;

const ArticleModel = new Lang.Class({
    Name: "ArticleModel",
    Extends: GObject.Object,
   	Properties: {
        'title': GObject.ParamSpec.string('title', 'Article Title', 'Human Readable Article Title',
            GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT_ONLY,
                    ""),
        'source': GObject.ParamSpec.string('source', 'Source',
            'Source website or database that the article is from',
            GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT_ONLY,
            ''),
        'uri': GObject.ParamSpec.string('uri', 'Article URI', 'Title URI as stored in wikipedia database', 
            GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT_ONLY,
                    "")
    },

    _init: function(params) {
        this._categoryList = [];
        this.parent(params);
    },

    /**
     * Method: setCategories
     * Give this article a list of categories or tags
     *
     * Parameters:
     *   categoryList - an array of strings signifying category IDs
     *
     * Makes this article's categories the list signified by _categoryList_,
     * wiping out any categories that were previously set.
     */
    setCategories: function (categoryList) {
        this._categoryList = categoryList;
    },

    /**
     * Method: addCategory
     * Attach a category or tag to this article
     *
     * Parameters:
     *   categoryId - a string signifying a category ID
     *
     * Tags this article with _categoryID_.
     * Does nothing if this article is already tagged with that ID.
     */
    addCategory: function (categoryId) {
        if (!this.hasCategory(categoryId))
            this._categoryList.push(categoryId);
    },

    /**
     * Method: getCategories
     * List of this article's categories or tags
     *
     * Returns:
     *   An array of strings signifying category IDs.
     */
    getCategories: function () {
        return this._categoryList;
    },

    /**
     * Method: hasCategory
     * Whether this article is tagged with a particular ID
     *
     * Parameters:
     *   categoryId - a string signifying a category ID
     *
     * Returns:
     *   true if this article is tagged with _categoryId_, false if not.
     */
    hasCategory: function (categoryId) {
        return this._categoryList.indexOf(categoryId) != -1;
    }

});

/**
 * Function: newFromJson
 * Construct a new <ArticleModel> from data exported by the CMS
 *
 * Parameters:
 *   json - a category object created by parsing the JSON file
 *
 * Returns:
 *   A newly created <ArticleModel>.
 *
 * See <DomainWikiModel.loadFromJson> for the structure of the JSON object.
 */
function newFromJson(json) {
    let retval = new ArticleModel({
        title: json['title'],
        source: json['source'],
        uri: json['url']
    });
    retval.setCategories(json['categories']);
    return retval;
}
