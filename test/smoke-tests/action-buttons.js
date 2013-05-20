// Copyright 2013 Endless Mobile, Inc.

const Lang = imports.lang;
const Endless = imports.gi.Endless;
const Gtk = imports.gi.Gtk;

const TEST_APPLICATION_ID = 'com.endlessm.example.test-action-buttons';

const TestApplication = new Lang.Class ({
    Name: 'TestApplication',
    Extends: Endless.Application,

    vfunc_startup: function() {
        this.parent();

        this._page = new Gtk.Grid();
        
        /* should be using Endless.EOS_ACTION_BUTTON_SIZE_PRIMARY */

        this._eosButton0 = new Endless.ActionButton({size: 0, label: 'SMILE', 'icon-id': 'face-smile-symbolic' });
        this._page.attach(this._eosButton0, 0, 0, 1, 1);
        
        this._eosButton1 = new Endless.ActionButton({size: 1, label: 'POUT', 'icon-id': 'face-sad-symbolic' });
        this._page.attach(this._eosButton1, 0, 1, 1, 1);
        
        this._eosButton2 = new Endless.ActionButton({size: 2, label: '', 'icon-id': 'edit-delete-symbolic' });
        this._page.attach(this._eosButton2, 0, 2, 1, 1);
        
        this._eosButton3 = new Endless.ActionButton({size: 3, label: '', 'icon-id': 'object-select-symbolic' });
        this._page.attach(this._eosButton3, 0, 3, 1, 1);
        
        this._pm = new Endless.PageManager();
        this._pm.add(this._page, { name: "page" });
        
        let provider = new Gtk.CssProvider ();
        provider.load_from_path ('./test/smoke-tests/eosactionbutton.css');
        
        this._window = new Endless.Window({
            application: this,
            border_width: 16,
            page_manager: this._pm
        });
        
        let context = new Gtk.StyleContext();
        context.add_provider_for_screen(this._window.get_screen(),
                                        provider,
                                        Gtk.STYLE_PROVIDER_PRIORITY_USER);
        
        this._window.show_all();
    },

    _onButtonClicked: function () {
        this._window.destroy();
    },
});

let app = new TestApplication({ application_id: TEST_APPLICATION_ID,
                                flags: 0 });
app.run(ARGV);
