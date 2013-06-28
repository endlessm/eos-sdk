//Copyright 2013 Endless Mobile, Inc.

const Lang = imports.lang;
const Endless = imports.gi.Endless;
const GLib = imports.gi.GLib;
const Gtk = imports.gi.Gtk;
const WebKit = imports.gi.WebKit;

const TEST_APPLICATION_ID = 'com.endlessm.example.test-webview';

const EOS_URI_SCHEME = 'endless://';

const TestApplication = new Lang.Class({
    Name: 'TestApplication',
    Extends: Endless.Application,

    /* *** CONVENIENCE LIBRARY *** */

    _onLoadStatus: function (web_view) {
        if (web_view.load_status == WebKit.LoadStatus.FINISHED) {
            // now we translate to Brazilian Portuguese
            this._translateHTML (web_view, 'pt_BR');
        }
    },

    _onNavigationPolicyDecisionRequested: function(web_view, frame, request,
                                                   navigation_action, policy_decision,
                                                   user_data) {
        // lame way of getting function name and parameters, without error check

        let uri = request.get_uri();

        if(uri.indexOf(EOS_URI_SCHEME) == 0) {
            // get the name and parameters for the desired function
            print('URI: '+uri);
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

            if(this._html_actions[function_name])
                Lang.bind(this, this._html_actions[function_name])(parameters);
            else
                print('Unknown function '+function_name);

            policy_decision.ignore();
            return true;
        } else {
            return false;
        }
    },

    _getElementById: function(webview, id) {
        // WebKit.DOMDocument
        let dom = webview.get_dom_document();

        // WebKit.DOMElement
        return dom.get_element_by_id(id);
    },

    _translateHTML: function(webview, lang) {
        let dom = webview.get_dom_document();

        // WebKit.DOMNodeList
        let translatable = dom.get_elements_by_name('translatable');

        for (var i = 0 ; i < translatable.get_length() ; i++) {
            // WebKit.DOMNode
            let element = translatable.item(i);

            // TODO here is where we would do the translation
            element.inner_html = '<i>' + element.inner_text + '</i>';
        }
    },

    /* *** ACTIONS AVAILABLE FROM HTML *** */

    _html_actions: {
        /* dict['name'] is the name of the page to move to */
        'moveToPage': function(dict) {
            print('move to page '+dict['name']);
            this._pm.visible_page_name = dict['name'];
        },

        /* dict['msg'] is the message to display */
        'showMessageFromParameter': function(dict) {
            let dialog = new Gtk.MessageDialog({
                buttons: Gtk.ButtonsType.CLOSE,
                message_type: Gtk.MessageType.INFO,
                text: dict['msg'] });
            dialog.set_transient_for(this._window);
            dialog.run();
            dialog.destroy();
        },

        /* dict['id'] is the ID of the input field to use */
        'showMessageFromInputField': function(dict) {
            let input = this._getElementById(this._webview, dict['id']);

            // WebKitDOMHTMLInputElement
            let msg = input.get_value();

            let dialog = new Gtk.MessageDialog({
                buttons: Gtk.ButtonsType.CLOSE,
                message_type: Gtk.MessageType.INFO,
                text: msg });
            dialog.set_transient_for(this._window);
            dialog.run();
            dialog.destroy();
        },

        /* dict['id'] is the ID of the element to use */
        'addStars': function(dict) {
            let e = this._getElementById(this._webview, dict['id']);
            e.inner_text += '★ ';
        }
    },

    /* *************************** */

    vfunc_startup: function() {
        this.parent();

        this._webview = new WebKit.WebView();

        let cwd = GLib.get_current_dir();
        let target = cwd + '/test/smoke-tests/webview/first_page.html';
        this._webview.load_uri(GLib.filename_to_uri(target, null));

        this._webview.connect('notify::load-status', 
                              Lang.bind(this, this._onLoadStatus));

        this._webview.connect('navigation-policy-decision-requested', 
                              Lang.bind(this, this._onNavigationPolicyDecisionRequested));

        this._page1 = new Gtk.ScrolledWindow();
        this._page1.add(this._webview);

        this._page2 = new Gtk.Grid();
        let back_button = new Gtk.Button({label:"Go back to page 1"});
        back_button.connect('clicked', Lang.bind(this, function() {
            this._pm.visible_page_name = 'page1';
        }))
        this._page2.add(back_button);

        this._pm = new Endless.PageManager();
        this._pm.set_transition_type(Endless.PageManagerTransitionType.CROSSFADE)
        this._pm.add(this._page1, { name: 'page1' });
        this._pm.add(this._page2, { name: 'page2' });

        this._pm.visible_page = this._page1;

        this._window = new Endless.Window({
            application: this,
            border_width: 16,
            page_manager: this._pm
        });

        this._window.show_all();
    }
});

let app = new TestApplication({ application_id: TEST_APPLICATION_ID,
    flags: 0 });
app.run(ARGV);
