// Copyright 2013 Endless Mobile, Inc.

const Lang = imports.lang;
const Endless = imports.gi.Endless;

const TEST_APPLICATION_ID = 'com.endlessm.example.test';

const TestApplication = new Lang.Class ({
    Name: 'TestApplication',
    Extends: Endless.Application,

    vfunc_startup: function() {
        this.parent();
        this._window = new Endless.Window({application: this});
        this._window.show_all();
    },
});

let app = new TestApplication({ application_id: TEST_APPLICATION_ID,
                                flags: 0 });
app.run(ARGV);
