// Copyright (C) 2016 Endless Mobile, Inc.

const Endless = imports.gi.Endless;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;

Gtk.init(null);

describe('TopbarNavButton', function () {
    it('works correctly with no_show_all', function () {
        let button = new Endless.TopbarNavButton({
            no_show_all: true,
        });
        button.show();
        expect(button.visible).toBe(true);
        expect(button.back_button.visible).toBe(true);
        expect(button.forward_button.visible).toBe(true);
    });
});
