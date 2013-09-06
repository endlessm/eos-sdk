const Endless = imports.gi.Endless;
const GLib = imports.gi.GLib;
const Gtk = imports.gi.Gtk;
const WebKit = imports.gi.WebKit;

const EOS_URI_SCHEME = 'endless://';


const Application = new Lang.Class({
    Name: 'WebApplication',
    Extends: Endless.Application,

//  Set of actions that may be invoked from a WebView.
//  Declare them as function(dict), and use links with the format
//  "endless://actionName?parameter=value"

    _webActions: { },

    // This callback does the translation from URI to action
    // webview.connect('navigation-policy-decision-requested',
    //     Lang.bind(this, this.web_actions_handler));

    web_actions_handler: function(webview, frame, request, action, policy_decision) {
        let uri = request.get_uri();

        if(uri.indexOf(EOS_URI_SCHEME) == 0) {
            // get the name and parameters for the desired function
            let f_call = uri.substring(EOS_URI_SCHEME.length, uri.length).split('?');
            var function_name = f_call[0];
            var parameters = {};

            if(f_call[1]) {
                // there are parameters
                let params = f_call[1].split('&');
                params.forEach(function(entry) {
                    let param = entry.split('=');

                    if(param.length == 2) {
                        param[0] = decodeURIComponent(param[0]);
                        param[1] = decodeURIComponent(param[1]);
                        // and now we add it...
                        parameters[param[0]] = param[1];
                    }
                });
            }

            if(this._webActions[function_name])
                Lang.bind(this, this._webActions[function_name])(parameters);
            else
                print('Unknown function '+function_name);

            policy_decision.ignore();
            return true;
        } else {
            // this is a regular URL, just navigate there
            return false;
        }
    },

//  convenience functions

    _getElementById: function(webview, id) {
        // WebKit.DOMDocument
        let dom = webview.get_dom_document();

        // WebKit.DOMElement
        return dom.get_element_by_id(id);
    },

    translate_html: function(webview) {
        let dom = webview.get_dom_document();

        // WebKit.DOMNodeList
        let translatable = dom.get_elements_by_name('translatable');

        for (var i = 0 ; i < translatable.get_length() ; i++) {
            // WebKit.DOMNode
            let element = translatable.item(i);

            // TODO here is where we would do the translation
            element.inner_html = '<i>' + element.inner_text + '</i>';
        }
    }
});
