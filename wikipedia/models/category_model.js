const Endless = imports.gi.Endless;
const GObject = imports.gi.GObject;
const Lang = imports.lang;

GObject.ParamFlags.READWRITE = GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE;

const CategoryModel = new Lang.Class({
    Name: "CategoryModel",
    Extends: GObject.Object,
   	Properties: {
        /**
         * Property: id
         * String for referring to this category internally
         *
         * Can generally be equal to <CategoryModel.title>, though that is not
         * required.
         */
        'id': GObject.ParamSpec.string('id', 'ID string',
            'String for referring to this category internally',
            GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT_ONLY,
            ''),
        'description': GObject.ParamSpec.string('description', 'Category Description', 'This is the text that the user reads on the category page.',
            GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT_ONLY,
                ""),
        'title': GObject.ParamSpec.string('title', 'Category Name', 'This is the name that is displayed on the front page and as the title on the category page.', 
            GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT_ONLY,
                ""),
        'image-uri': GObject.ParamSpec.string('image-uri', 'Category Image URI', 'Path to image for this category in the GResource', 
            GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT_ONLY,
                ""),
        'image-thumbnail-uri': GObject.ParamSpec.string('image-thumbnail-uri', 'Category Thumbnail Image URI', 'Path to thumbnail image for this category in the GResource', 
            GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT_ONLY,
                ""),
        'is-main-category': GObject.ParamSpec.boolean('is-main-category', 'Is Main Category boolean', 'Flag denoting whether this category is the main category for this app', 
            GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT_ONLY,
                false),
        /**
         * Property: has_articles
         * Whether this category contains articles
         *
         * This property is not computed in any way, but is set by the library
         * model when loading from a JSON file.
         * This value is used to determine whether a category should be
         * clickable, for example.
         */
        'has-articles': GObject.ParamSpec.boolean('has-articles',
            'Has articles',
            'Indicates whether category has any articles under it',
            GObject.ParamFlags.READWRITE,
            false)
    },

    _init: function(params) {
        this._subcategories = {};
        this.parent(params);
    },

    /**
     * Method: addSubcategory
     * Add a subcategory to this model
     *
     * Parameters:
     *   modelObj - another <CategoryModel> that is to be a subcategory of this
     *   one.
     *
     * Does nothing if this category already has a subcategory with _modelObj_'s
     * ID.
     */
    addSubcategory: function (modelObj) {
        if (!this._subcategories.hasOwnProperty(modelObj.id)) {
            this._subcategories[modelObj.id] = modelObj;
        }
    },

    /**
     * Method: getSubcategories
     * List this model's subcategories
     *
     * Returns:
     *   An array of <CategoryModels> representing this category's
     *   subcategories, or an empty array if there are none.
     */
    getSubcategories: function () {
        let retval = [];
        for (let id in this._subcategories) {
            retval.push(this._subcategories[id]);
        }
        return retval;
    }
});

/**
 * Function: newFromJson
 * Construct a new <CategoryModel> from data exported by the CMS
 *
 * Parameters:
 *   json - a category object created by parsing the JSON file
 *
 * Returns:
 *   A newly created <CategoryModel> with no subcategories.
 *
 * See <DomainWikiModel.loadFromJson> for the structure of the JSON object.
 */
function newFromJson(json) {
    let retval = new CategoryModel({
        id: json['category_name'],
        description: json['content_text'],
        title: json['category_name'],
        image_uri: json['image_file'],
        image_thumbnail_uri: json['image_thumb_uri'],
        is_main_category: json['is_main_category']
    });
    return retval;
}
