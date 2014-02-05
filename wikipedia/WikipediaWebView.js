const Gdk = imports.gi.Gdk;
const Gio = imports.gi.Gio;
const GLib = imports.gi.GLib;
const Gtk = imports.gi.Gtk;
const GObject = imports.gi.GObject;
const Lang = imports.lang;
const WebKit = imports.gi.WebKit2;
const Utils = imports.wikipedia.utils;

const API_ENDPOINT = "http://127.0.0.1:3000";
const API_VERSION = "v1";
const getPageByIdURI = "getArticleById?";
const getPageByTitleURI = "getArticleByTitle?";
const getPageByQueryURI = "getTopArticleByQuery?";
const getTitlesByQueryURI = "getArticleTitlesByQuery?";

// Interpret image:// URIs as wikipedia images
WebKit.WebContext.get_default().register_uri_scheme('image', function(request) {
    let filename = request.get_uri().slice('image://'.length);
    filename = decodeURI(filename);
    let pictures_dir = request.get_web_view()._getArticleImagesPath();
    let parent = Gio.File.new_for_path(pictures_dir);
    let file = parent.get_child(filename);
    try {
        let stream = file.read(null);
        request.finish(stream, -1, 'image/png');
    } catch (err) {
        let stream = new Gio.MemoryInputStream();
        request.finish(stream, 0, 'image/png');
    }
});

const WikipediaWebView = new Lang.Class({
    Name: 'EndlessWikipediaWebView',
    Extends: WebKit.WebView,
    Properties: {
        'hide-links': GObject.ParamSpec.boolean('hide-links',
            'Hide article links',
            'A boolean to determine whether links should be shown',
            GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT,
            false),
        'system-personality': GObject.ParamSpec.string('system-personality',
            'System Personality string',
            'Specifies the system personality to be used in this wiki webview',
            GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT,
            ""),
        'app-name': GObject.ParamSpec.string('app-name',
            'Application name',
            'Specifies the application that is using this wiki webview',
            GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT,
            ""),
    },

    _init: function(params) {
        this.parent(params);
        // For debugging
        //let settings = this.get_settings();
        //settings.set_enable_developer_extras(true);
        //this.set_settings(settings);
        this.connect('context-menu', Lang.bind(this, function() {return true;}));

        this.connect('decide-policy',
            Lang.bind(this, this._onNavigation));
        this.connect('load-changed',
            Lang.bind(this, this._onLoadChange));
    },

    setShowableLinks: function(linked_articles){
        this._links_to_show = linked_articles;
    },

    _getFullURL: function(method, params){
        // We always include personality and
        // app name on all requests.
        let base_url = [API_ENDPOINT, API_VERSION, method].join("/");
        params["personality"] = this.system_personality;
        params["appname"] = this.app_name;
        let full_url = base_url;
        for(let key in params){
            full_url += key + "=" + params[key] + "&";
        }
        // Remove the final '&'
        full_url = full_url.slice(0, -1);
        return full_url;
    },

    loadArticleById: function (id) {
        let params = {
            id: id,
            hideLinks: this.hide_links
        };
        let url = this._getFullURL(getPageByIdURI, params);
        this.load_uri(url);
    },

    loadArticleBySearchQuery: function (query, source) {
        let params = {
            query: query,
            hideLinks: this.hide_links,
            source: source
        };
        let url = this._getFullURL(getPageByQueryURI, params);
        this.load_uri(url);
    },

    loadTitlesBySearchQuery: function (query) {
        let params = { query: query };
        let url = this._getFullURL(getTitlesByQueryURI, params);
        this.load_uri(url);
    },

    scriptFinished: function(){
        // NO OP
    },

    setAllowedLinks: function(){
        // If you want to show all links, then
        // no point in showing some subset of them as well
        if(!this.hide_links || this._links_to_show.length === 0){
            return;
        }
        let str = JSON.stringify(this._links_to_show);
        let script = "window.links_to_show = " + str;
        this.run_javascript(script, null, this.scriptFinished, null);
    },

    _getArticleImagesPath: function() {
        let cur_exec = this.get_toplevel().get_application().application_base_path;
        return cur_exec + "/web_view/article_images/";
    },

    _onNavigation: function(webview, decision, decision_type) {
        if (decision_type == WebKit.PolicyDecisionType.NAVIGATION_ACTION) {
            let uri = decision.request.uri;
            if (uri.startsWith(API_ENDPOINT + "/wiki/")) {
                let parts = uri.split("/");
                let suffix = parts[parts.length - 1];
                let id = decodeURI(suffix);
                this.loadArticleById(id);
                decision.ignore();
                return true; // handled
            } else if (GLib.uri_parse_scheme(uri).startsWith('browser-')) {
                // Open everything that starts with 'browser-' in the system
                // browser
                let realURI = uri.slice('browser-'.length);
                Gtk.show_uri(null, realURI, Gdk.CURRENT_TIME);
                decision.ignore();
                return true; // handled
            }
        }
        return false; // not handled, default behavior
    },

    _onLoadChange: function(webview, load_event, data){
        this.setAllowedLinks();
    }
});
