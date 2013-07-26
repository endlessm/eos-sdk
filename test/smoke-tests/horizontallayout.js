// Copyright 2013 Endless Mobile, Inc.

const Lang = imports.lang;
const Endless = imports.gi.Endless;
const Gtk = imports.gi.Gtk;

const TEST_APPLICATION_ID = 'com.endlessm.example.test-horizontal-layout';

const ANIMATION_DURATION = 2 * 1000000; // 2 seconds

const TestApplication = new Lang.Class ({
    Name: 'TestApplication',
    Extends: Endless.Application,

    animate_scroll: function(widget, forward) {
        this._start_time = widget.get_frame_clock().get_frame_time();

        widget.add_tick_callback(Lang.bind (this, function(widget, frameClock) {
            let hadj = widget.get_hadjustment();
            let start = hadj.get_lower ();
            let end = hadj.get_upper() - hadj.get_page_size();
            let value = hadj.get_value();
            let now = frameClock.get_frame_time() - this._start_time;
            
            if (forward) {
                hadj.set_value (end * now/ANIMATION_DURATION);
            } else {
                hadj.set_value (end * (1 - now/ANIMATION_DURATION));
            }
            return (now < ANIMATION_DURATION);
        }));
    },
    
    vfunc_startup: function() {
        this.parent();

        this._page = new Gtk.ScrolledWindow({ name: "page",
            'vscrollbar-policy': Gtk.PolicyType.NEVER,
            valign: Gtk.Align.FILL});
        
        this._content = new Gtk.Grid({'column-homogeneous': true,
                                      'column-spacing': 100,
                                      vexpand: true,
                                      valign: Gtk.Align.FILL});
        
        this._goButton = new Gtk.Button({label: 'GO', valign: Gtk.Align.CENTER});
        this._content.add(new Gtk.Frame({child: this._goButton}));
        
        this._content.add(new Gtk.Frame());
        this._content.add(new Gtk.Frame());
        this._content.add(new Gtk.Frame());
        
        this._backButton = new Gtk.Button({label: 'BACK', valign: Gtk.Align.CENTER});
        this._content.add(new Gtk.Frame({child: this._backButton}));

        this._page.add(this._content);

        this._hadjustment = this._page.get_hadjustment();
        this._css = new Gtk.CssProvider();
        this._page.get_style_context().add_provider (this._css, Gtk.STYLE_PROVIDER_PRIORITY_USER);

        this._goButton.connect('clicked', Lang.bind (this, function () {
            this.animate_scroll(this._page, true);
            this._css.load_from_data('#page { background-position: -500px 0; }');
        }));
        
        this._backButton.connect('clicked', Lang.bind (this, function () {
            this.animate_scroll(this._page, false);
            this._css.load_from_data('#page { background-position: 0px 0; }');
        }));
        
        this._pm = new Endless.PageManager();
        this._pm.add(this._page, { name: "page" });
        
        let provider = new Gtk.CssProvider ();
        provider.load_from_path ('./test/smoke-tests/horizontallayout.css');
        
        this._window = new Endless.Window({
            application: this,
            border_width: 1,
            page_manager: this._pm
        });
        
        let context = new Gtk.StyleContext();
        context.add_provider_for_screen(this._window.get_screen(),
                                        provider,
                                        Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION);
        
        this._window.show_all();
    },

    _onButtonClicked: function () {
        this._window.destroy();
    },
});

let app = new TestApplication({ application_id: TEST_APPLICATION_ID,
                                flags: 0 });
app.run(ARGV);
