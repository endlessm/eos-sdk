const Gtk = imports.gi.Gtk;
const Lang = imports.lang;
const Soup = imports.gi.Soup;
const WebKit = imports.gi.WebKit;

const getPageURL = "http://localhost:3000/getArticleByTitle?title=";

const WikipediaView = new Lang.Class({
    Name: 'EndlessWikipediaView',
    Extends: WebKit.WebView,

    _init: function(params) {
        this._httpSession = new Soup.Session();
        this.parent(params);
    },

    loadArticleByTitle: function(title) {
        let request = Soup.Message.new("GET", getPageURL + title);
        this._httpSession.queue_message(request, Lang.bind(this, function(_httpSession, message) {
            let articleJSON = request.response_body.data;
            let article = JSON.parse(articleJSON);
            let articleHTML = article["text"];
            this.load_string(articleHTML, "text/html", "UTF-8", "");
            //the last argument is the base-uri, we might need to change this based
            //on our local image directory, I'm not sure.
        }));
    }
});
