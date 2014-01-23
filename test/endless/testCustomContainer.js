const Endless = imports.gi.Endless;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;

const TestContainer = new Lang.Class({
    Name: 'TestContainer',
    Extends: Endless.CustomContainer,

    _init: function (params) {
        this.parent(params);
    },

    vfunc_size_allocate: function (alloc) {
        this.parent(alloc);
    }
});

describe("CustomContainer", function () {
    it("Instantiates a CustomContainer subclass to make sure no exceptions/segfaults", function () {
        let createContainer = function () {
            let container = new TestContainer();
        };

        expect(createContainer).not.toThrow();
    });
});
