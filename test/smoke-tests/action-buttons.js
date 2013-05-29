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

        this._page = new Endless.ActionMenu();
        
        this._page.add_action (new Gtk.Action({
            name: 'select',
            'icon-name': 'object-select-symbolic',
            label: 'select stuff',
            'is-important': true }));
        
        this._page.get_action('select').connect('activate',
            Lang.bind(this, function () {
        	var md = new Gtk.MessageDialog({modal:true, title:"Information",
        	    message_type:Gtk.MessageType.INFO,
        	    buttons:Gtk.ButtonsType.OK, text:"Select button pressed!"});
        	md.run();
        	md.destroy();
            }));

        this._page.add_action (new Gtk.Action({
            name: 'delete',
            'icon-name': 'edit-delete-symbolic',
            label: 'delete stuff',
            'is-important': false }));

        this._page.add_action (new Gtk.Action({
            name: 'smile',
            'icon-name': 'face-smile-symbolic',
            label: 'smile',
            'is-important': false }));

        this._page.add_action (new Gtk.Action({
            name: 'sadface',
            'icon-name': 'face-sad-symbolic',
            label: 'sadface',
            'is-important': false }));

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
