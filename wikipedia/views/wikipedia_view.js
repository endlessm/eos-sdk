const Gio = imports.gi.Gio;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;
const Soup = imports.gi.Soup;
const WebKit = imports.gi.WebKit2;
const Utils = imports.utils;

const getPageURL = "http://127.0.0.1:3000/getArticleByTitle?title=";

// Interpret image:// URIs as wikipedia images
WebKit.WebContext.get_default().register_uri_scheme('image', function(request) {
    let filename = request.get_uri().slice('image://'.length);
    let pictures_dir = request.get_web_view()._getArticleImagesPath();
    let parent = Gio.File.new_for_path(pictures_dir);
    let file = parent.get_child(filename);
    let stream = file.read(null);
    request.finish(stream, -1, 'image/png');
});

const WikipediaView = new Lang.Class({
    Name: 'EndlessWikipediaView',
    Extends: WebKit.WebView,

    _init: function(params) {
        this.parent(params);
        // For debugging
        //let settings = this.get_settings();
        //settings.set_enable_developer_extras(true);
        //this.set_settings(settings);
        this.connect('context-menu', Lang.bind(this, function(){return true}));
    },

    loadArticleByTitle: function(url) {
        let parts = url.split("/");
        let suffix = parts[parts.length - 1];
        let title = decodeURI(suffix.replace("_", " ", 'g'));
        this.load_uri(getPageURL + title);
    },

    _getArticleImagesPath: function() {
        let cur_exec = this.get_toplevel().get_application().application_base_path;
        return cur_exec + "/web_view/article_images/";
    }
});
