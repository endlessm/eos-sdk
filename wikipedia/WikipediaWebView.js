const Gio = imports.gi.Gio;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;
const Soup = imports.gi.Soup;
const WebKit = imports.gi.WebKit2;
const Utils = imports.wikipedia.utils;

const hostName = "http://127.0.0.1:3000/"
const getPageURI = "getDomainSpecificArticle?title=";

// Interpret image:// URIs as wikipedia images
WebKit.WebContext.get_default().register_uri_scheme('image', function(request) {
    let filename = request.get_uri().slice('image://'.length);
    filename = decodeURI(filename);
    let pictures_dir = request.get_web_view()._getArticleImagesPath();
    let parent = Gio.File.new_for_path(pictures_dir);
    let file = parent.get_child(filename);
    let stream = file.read(null);
    request.finish(stream, -1, 'image/png');
});

const WikipediaWebView = new Lang.Class({
    Name: 'EndlessWikipediaWebView',
    Extends: WebKit.WebView,

    _init: function(params) {
        this.parent(params);
        // For debugging
        //let settings = this.get_settings();
        //settings.set_enable_developer_extras(true);
        //this.set_settings(settings);
        this.connect('context-menu', Lang.bind(this, function(){return true}));

        this.connect('decide-policy',
            Lang.bind(this, this._onNavigation));
    },

    loadArticleByURI: function(uri) {
        let parts = uri.split("/");
        let suffix = parts[parts.length - 1];
        let title = decodeURI(suffix.replace("_", " ", 'g'));
        this.load_uri(hostName + getPageURI + title);
    },

    loadArticleByTitle: function(title) {
        this.load_uri(hostName + getPageURI + title);
    },

    _getArticleImagesPath: function() {
        let cur_exec = this.get_toplevel().get_application().application_base_path;
        return cur_exec + "/web_view/article_images/";
    },

    _onNavigation: function(webview, decision, decision_type) {
        if (decision_type == WebKit.PolicyDecisionType.NAVIGATION_ACTION) {
            let uri = decision.request.uri;
            if (uri.startsWith(hostName + "wiki/")) {
                let parts = uri.split("/");
                let suffix = parts[parts.length - 1];
                let title = decodeURI(suffix.replace("_", " ", 'g'));
                this.loadArticleByTitle(title);
                return true;
            }
        }
        return false; // not handled, default behavior
    }
});
