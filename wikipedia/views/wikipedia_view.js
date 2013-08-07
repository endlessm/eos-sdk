const Gtk = imports.gi.Gtk;
const Lang = imports.lang;
const Soup = imports.gi.Soup;
const WebKit = imports.gi.WebKit2;
const Utils = imports.utils;

const getPageURL = "http://127.0.0.1:3000/getArticleByTitle?title=";

const WikipediaView = new Lang.Class({
    Name: 'EndlessWikipediaView',
    Extends: WebKit.WebView,

    _init: function(params) {
        this._httpSession = new Soup.Session();
        this.parent(params);
        // For debugging
        //let settings = this.get_settings();
        //settings.set_enable_developer_extras(true);
        //this.set_settings(settings);
        this.connect('context-menu', Lang.bind(this, function(){return true}));
    },

    _get_body_html:function(articleHTML, title, image_path){
        let html = "";
        html += "<div id='main'>";
        html += "<section id='main-content'>";
        html += "<header id='header-content'>";
        html += "<h1>" + title + "</h1>";
        html += "</header>";
        html += "<section image_path=" + image_path +" id='inside-content'>";
        html += "<hr class='hr-title'>";
        html += articleHTML;
        html += "</section>";
        html += "</section>";
        html += "</div>";
        return html;
    },

    _get_style_sheet_html: function(current_dir, sheets){
        let html = "";
        for(let i = 0; i < sheets.length; i++){
            html += "<link rel='stylesheet' href=" + current_dir + "/css/" + sheets[i] + ">";
        }
        return html;
    },

    _get_script_html:function(current_dir, scripts){
        let html = "";
        for(let i = 0; i < scripts.length; i++){
            html += "<script src='" + current_dir + "/js/" + scripts[i] + "'></script>";
        }
        return html;
    },

    _get_meta_html:function(){
        let html = "";
        html += "<meta charset='utf-8'>";
        html += "<meta http-equiv='X-UA-Compatible' content='IE=edge,chrome=1'>";
        html += "<meta name='description' content=''>";
        html += "<title></title>";
        html += "<meta name='viewport' content='width=device-width'>";
        return html;
    },

    loadArticleByTitle: function(url, human_title) {
        let parts = url.split("/");
        let suffix = parts[parts.length - 1];
        let title = decodeURI(suffix.replace("_", " ", 'g'));
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
            let current_dir = Endless.getCurrentFileDir();

            let cur_exec = this.get_toplevel().get_application().application_base_path;
            let image_path = cur_exec + "/web_view/article_images/";

            let documentHTML = this._get_meta_html() + this._get_body_html(articleHTML, human_title, image_path);

            let sheets = new Array("first_load.css", "second_load.css","main.css","wikipedia.css","nolinks.css");
            documentHTML = this._get_style_sheet_html(current_dir, sheets) + documentHTML;

            let scripts = new Array("jquery-min.js", "main.js");
            documentHTML = documentHTML + this._get_script_html(current_dir, scripts);

            let temp_uri = Utils.write_contents_to_temp_file("wiki.html", documentHTML);
            // TODO: Ask about how we can load directly from HTML using load_html. 
            // Right now, this doesn't work, regardless of what we put in for base_uri
            this.load_uri(temp_uri);
        }));
    }
});
