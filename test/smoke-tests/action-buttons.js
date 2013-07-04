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

        this._page = new Gtk.Grid ();
        
        this._content = new Gtk.Grid ({
            hexpand: true,
            halign: Gtk.Align.CENTER,
            vexpand: true,
            valign: Gtk.Align.CENTER});

        this._darkSwitch = new Gtk.Switch ({active: false});
        this._darkSwitch.connect ('notify::active', Lang.bind (this, function (active) {
            if (this._darkSwitch.get_active()) {
                this._menu.get_style_context().add_class('dark');
            } else {
                this._menu.get_style_context().remove_class('dark');
            }
        }));
        this._content.add(new Gtk.Label ({label: 'Dark action menu'}), 0, 0, 1, 1);
        this._content.add(this._darkSwitch, 0, 1, 1, 1);
        
        this._menu = new Endless.ActionMenu ({name: 'menu'});
        
        // the ActionMenu takes 1/6 of the width
        this._page.set_column_homogeneous (true);
        this._page.attach (this._content, 0, 0, 5, 1);
        this._page.attach (this._menu, 5, 0, 1, 1);
        
        this._menu.add_action ({
            name: 'select',
            'icon-name': 'object-select-symbolic',
            label: 'select stuff',
            'is-important': true },
            Lang.bind(this, function () {
        	var md = new Gtk.MessageDialog({modal:true, title:"Information",
        	    message_type:Gtk.MessageType.INFO,
        	    buttons:Gtk.ButtonsType.OK, text:"Select button pressed!"});
        	md.run();
        	md.destroy();
            }));

        this._menu.add_action ({
            name: 'delete',
            'icon-name': 'edit-delete-symbolic',
            label: 'delete stuff',
            'is-important': false });

        this._menu.add_action ({
            name: 'smile',
            'icon-name': 'face-smile-symbolic',
            label: 'smile',
            'is-important': false });

        this._menu.add_action ({
            name: 'sadface',
            'icon-name': 'face-sad-symbolic',
            label: 'sadface',
            'is-important': false });

        this._pm = new Endless.PageManager();
        this._pm.add(this._page, { name: "page" });
        
        let provider = new Gtk.CssProvider ();
        provider.load_from_path ('./test/smoke-tests/eosactionbutton.css');
        
        this._window = new Endless.Window({
            application: this,
            border_width: 1,
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
