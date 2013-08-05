const Gtk = imports.gi.Gtk;
const Lang = imports.lang;
const Soup = imports.gi.Soup;
const WebKit = imports.gi.WebKit2;
const Utils = imports.utils;

const getPageURL = "http://localhost:3000/getArticleByTitle?title=";
const PATH_TO_INDEX = "web_view/index.html";
const PATH_TO_TEMP = "web_view/temp.html";

const WikipediaView = new Lang.Class({
    Name: 'EndlessWikipediaView',
    Extends: WebKit.WebView,

    _init: function(params) {
        this._httpSession = new Soup.Session();
        this.parent(params);
        // For debugging
        // let settings = this.get_settings();
        // settings.set_enable_developer_extras(true);
        // this.set_settings(settings);
        this._is_first_time = true;
    },

    loadArticleByTitle: function(title) {
        let request = Soup.Message.new("GET", getPageURL + title);
        this._httpSession.queue_message(request, Lang.bind(this, function(_httpSession, message) {
            if(message.status_code !== 200) {
                print(message.status_code);
                return;
            }
            let articleJSON = request.response_body.data;
            let article = JSON.parse(articleJSON);
            let articleHTML = article["text"];
            let title = article['title'];
            let skeletonHTML = Utils.load_file(PATH_TO_INDEX);
            skeletonHTML = skeletonHTML + "<div id='wiki_content' name='"+ title +"'>" + articleHTML + "</div>"
            Utils.write_contents_to_file(PATH_TO_TEMP, skeletonHTML);
            // TODO: Ask about how we can load directly from HTML. Right now, WebKit can't seem to open 
            // CSS file correctly. All characters in CSS file are in Chinese
            let temp_uri = Utils.get_uri_for_relative_path(PATH_TO_TEMP)
            if(this._is_first_time) {
                this.load_uri(temp_uri);
                this._is_first_time = false;
            } else {
                this.reload();
            }
        }));
    }
});
