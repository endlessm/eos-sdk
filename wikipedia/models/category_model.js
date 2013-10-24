const Endless = imports.gi.Endless;
const GObject = imports.gi.GObject;
const Lang = imports.lang;

GObject.ParamFlags.READWRITE = GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE;

const CategoryModel = new Lang.Class({
    Name: "CategoryModel",
    Extends: GObject.Object,
   	Properties: {
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

    addSubcategory: function (modelObj) {
        if (!this._subcategories.hasOwnProperty(modelObj.id)) {
            this._subcategories[modelObj.id] = modelObj;
        }
    },

    getSubcategories: function () {
        let retval = [];
        for (let id in this._subcategories) {
            retval.push(this._subcategories[id]);
        }
        return retval;
    }
});

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
