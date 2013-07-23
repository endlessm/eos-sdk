const Endless = imports.gi.Endless;
const GObject = imports.gi.GObject;
const Lang = imports.lang;


const ArticleModel = new Lang.Class({
    Name: "ArticleModel",
    Extends: GObject.Object,
   	Properties: {
<<<<<<< HEAD
        'title': GObject.ParamSpec.string('title', 'Article Title', 'Human Readable Article Title',
            GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE | GObject.ParamFlags.CONSTRUCT,
                    ""),
        'uri': GObject.ParamSpec.string('uri', 'Article URI', 'Title URI as stored in wikipedia database', 
                GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE | GObject.ParamFlags.CONSTRUCT,
                    "")
=======
        'title': GObject.ParamSpec.string('title', 'Article Title', 'Article Title',
            GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE | GObject.ParamFlags.CONSTRUCT,
                                       ""),
        'uri': GObject.ParamSpec.string('uri', 'Article URI', 'Article URI', 
                                         GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE | GObject.ParamFlags.CONSTRUCT,
                                         "")
>>>>>>> 9a85677... Finished category and application model
    },

    _init: function(params) {
        this.parent(params);
    }

});