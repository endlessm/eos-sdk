// Copyright 2013 Endless Mobile, Inc.

const Lang = imports.lang;
const Endless = imports.gi.Endless;
const Gtk = imports.gi.Gtk;
const GObject = imports.gi.GObject;

const TEST_APPLICATION_ID = 'com.endlessm.example.test-horizontal-layout';

const ANIMATION_DURATION = 1 * 1000000; // 1 second

const TestApplication = new Lang.Class ({
    Name: 'TestApplication',
    Extends: Endless.Application,

    animate_scroll: function(widget, forward) {
        this._start_time = widget.get_frame_clock().get_frame_time();

        widget.add_tick_callback(Lang.bind (this, function(widget, frameClock) {
            let hadj = widget.get_hadjustment();
            let start = hadj.get_lower ();
            let end = hadj.get_upper() - hadj.get_page_size();
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
        
        this._goButton = new Gtk.Button({label: 'GO', valign: Gtk.Align.CENTER});
        this._goButton.connect('clicked', Lang.bind (this, function () {
            this.animate_scroll(this._page, true);
        }));

        this._backButton = new Gtk.Button({label: 'BACK', valign: Gtk.Align.CENTER});
        this._backButton.connect('clicked', Lang.bind (this, function () {
            this.animate_scroll(this._page, false);
        }));

        this._left = new Gtk.Frame({name: 'left',
                                    child: this._goButton,
                                    expand: true });
        
        this._center = new Gtk.Frame({name: 'center',
                                      vexpand: true,
                                      hexpand: false,
                                      'width-request': 256});

        this._right = new Gtk.Frame({name: 'right',
                                     child: this._backButton,
                                     expand: true });

        this._content = new Gtk.Grid({'column-spacing': 24,
                                      vexpand: true,
                                      valign: Gtk.Align.FILL});

        this._content.add(this._left);
        this._content.add(this._center);
        this._content.add(this._right);

        this._page = new Gtk.Viewport({name: "page",
                                       'shadow-type': Gtk.ShadowType.NONE,
                                       'border-width': 0,
                                       halign: Gtk.Align.FILL,
                                       valign: Gtk.Align.FILL});

        /*
         * Note: We call resize_children() because otherwise changes in the size
         * request of the viewport's child are not propagated immediately (which
         * is probably a GTK+ bug). Since this causes a new size allocation, we
         * should disable our own callback so that it is not called twice.
         * 
         * Another option would be to set Gtk.ResizeMode.IMMEDIATE in the
         * viewport, although this is theoretically deprecated.
         */
        
        this._page.add(this._content);
        this._size_allocate_handler = this._page.connect("size-allocate",
                                                         Lang.bind(this, function(widget, allocation) {
            let expanded_width = allocation.width - this._center.width_request - this._content.column_spacing;
            if (this._content.get_mapped() && expanded_width > 0) {
                this._content.width_request = 2*expanded_width + this._center.width_request + 2*this._content.column_spacing;
                //GObject.signal_handler_block(this._page, this._size_allocate_handler);        
                this._page.resize_children();
                //GObject.signal_handler_unblock(this._page, this._size_allocate_handler);
            }
        }));

        this._hadjustment = this._page.get_hadjustment();
        
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
