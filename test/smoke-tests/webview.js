//Copyright 2013 Endless Mobile, Inc.

const Lang = imports.lang;
const Endless = imports.gi.Endless;
const GLib = imports.gi.GLib;
const Gtk = imports.gi.Gtk;
const WebKit = imports.gi.WebKit;

// WebHelper.js must be copied somewhere in GJS's imports.searchPath
const WebHelper = imports.WebHelper; 

const TEST_APPLICATION_ID = 'com.endlessm.example.test-webview';

const TestApplication = new Lang.Class({
    Name: 'TestApplication',
    Extends: WebHelper.Application,

    /* *** ACTIONS AVAILABLE FROM THE WEB VIEW *** */

    _webActions: {
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
                              Lang.bind(this, function (web_view, status) {
                                  if (web_view.load_status == WebKit.LoadStatus.FINISHED) {
                                      // now we translate to Brazilian Portuguese
                                      this._translateHTML (web_view, 'pt_BR');
                                  }
                              }));

        this._webview.connect('navigation-policy-decision-requested', 
                              Lang.bind(this, this._onNavigationRequested));

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
