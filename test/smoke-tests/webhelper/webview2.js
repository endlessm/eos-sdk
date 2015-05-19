// Copyright 2015 Endless Mobile, Inc.

const Endless = imports.gi.Endless;
const Gettext = imports.gettext;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;
const WebHelper2 = imports.webhelper2;
const WebKit2 = imports.gi.WebKit2;

const TEST_APPLICATION_ID = 'com.endlessm.example.test-webview2';
const TEST_HTML = '\
<html> \
<head> \
<title>First page</title> \
<style> \
p, form { \
    width: 50%; \
    padding: 1em; \
    background: #FFFFFF; \
} \
body { \
    background: #EEEEEE; \
} \
</style> \
</head> \
\
<body> \
<h1>First page</h1> \
\
<p><a href="webhelper://moveToPage?name=page2">Move to page 2</a></p> \
\
<p><a \
href="webhelper://showMessageFromParameter?msg=This%20is%20a%20message%20from%20the%20URL%20parameter">Show \
message from parameter in this URL</a></p> \
\
<form action="webhelper://showMessageFromParameter"> \
<input name="msg" value="I am in a form!"/> \
<input type="submit" value="Show message using a form"/> \
</form> \
\
<p><a href="http://wikipedia.org">Regular link to a Web site</a></p> \
\
<p>This is text that will be italicized: <span name="translatable">Hello, \
world!</span></p> \
\
<p><button onclick="alert(gettext(\'I came from gettext\'));"> \
    Click me to use gettext \
</button></p> \
\
</body> \
</html>';

const TestApplication = new Lang.Class({
    Name: 'TestApplication',
    Extends: Endless.Application,

    vfunc_dbus_register: function (connection, object_path) {
        this._webhelper = new WebHelper2.WebHelper({
            well_known_name: this.application_id,
            connection: connection,
        });
        return this.parent(connection, object_path);
    },

    vfunc_startup: function () {
        this.parent();

        this._webhelper.set_gettext((s) => s.italics());
        this._webhelper.define_web_actions({
            moveToPage: this.moveToPage.bind(this),
            showMessageFromParameter: this.showMessageFromParameter.bind(this),
        });

        this._webview = new WebKit2.WebView();
        this._webview.connect('load-changed', (webview, event) => {
            if (event === WebKit2.LoadEvent.FINISHED)
                this._webhelper.translate_html(webview, null, (obj, res) => {
                    this._webhelper.translate_html_finish(res);
                });
        });
        this._webview.load_html(TEST_HTML, null);

        this._page2 = new Gtk.Grid();
        let back_button = new Gtk.Button({ label: 'Go back to page 1' });
        back_button.connect('clicked', () => {
            this._pm.visible_child_name = 'page1';
        });
        this._page2.add(back_button);

        this._window = new Endless.Window({
            application: this,
            border_width: 16,
        });

        this._pm = this._window.page_manager;
        this._pm.set_transition_type(Gtk.StackTransitionType.CROSSFADE);
        this._pm.add(this._webview, { name: 'page1' });
        this._pm.add(this._page2, { name: 'page2' });
        this._pm.visible_child_name = 'page1';

        this._window.show_all();
    },

    vfunc_dbus_unregister: function (connection, object_path) {
        this.parent(connection, object_path);
        this._webhelper.unregister();
    },

    // WEB ACTIONS

    // dict['name'] is the name of the page to move to
    moveToPage: function (dict) {
        this._pm.visible_child_name = dict['name'];
    },

    // dict['msg'] is the message to display
    showMessageFromParameter: function (dict) {
        let dialog = new Gtk.MessageDialog({
            buttons: Gtk.ButtonsType.CLOSE,
            message_type: Gtk.MessageType.INFO,
            text: dict['msg'],
            transient_for: this._window,
        });
        dialog.run();
        dialog.destroy();
    },
});

let app = new TestApplication({
    application_id: TEST_APPLICATION_ID,
});
app.run(ARGV);
