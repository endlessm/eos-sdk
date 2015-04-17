const Module = imports.tools['eos-application-manifest'].commands.init;

describe('eos-application-manifest init', function () {
    describe('argument parser', function () {
        it('parses one argument without a value', function () {
            let input = ['--parameter'];
            let expected = {
                parameter: null,
            };
            let actual = Module.parseRemainingArgs(input);
            expect(JSON.stringify(actual)).toEqual(JSON.stringify(expected));
        });

        it('parses one argument with a value', function () {
            let input = ['--parameter=value'];
            let expected = {
                parameter: 'value',
            };
            let actual = Module.parseRemainingArgs(input);
            expect(JSON.stringify(actual)).toEqual(JSON.stringify(expected));
        });

        it('parses one argument followed by a value', function () {
            let input = ['--parameter', 'value'];
            let expected = {
                parameter: 'value',
            };
            let actual = Module.parseRemainingArgs(input);
            expect(JSON.stringify(actual)).toEqual(JSON.stringify(expected));
        });

        it('parses two arguments without a value', function () {
            let input = ['--parameter-one', '--parameter-two'];
            let expected = {
                'parameter-one': null,
                'parameter-two': null,
            };
            let actual = Module.parseRemainingArgs(input);
            expect(JSON.stringify(actual)).toEqual(JSON.stringify(expected));
        });

        it('parses an argument with a value followed by an argument', function () {
            let input = ['--parameter-one=value', '--parameter-two'];
            let expected = {
                'parameter-one': 'value',
                'parameter-two': null,
            };
            let actual = Module.parseRemainingArgs(input);
            expect(JSON.stringify(actual)).toEqual(JSON.stringify(expected));
        });

        it('parses an argument followed by a value followed by an argument', function () {
            let input = ['--parameter-one', 'value', '--parameter-two'];
            let expected = {
                'parameter-one': 'value',
                'parameter-two': null,
            };
            let actual = Module.parseRemainingArgs(input);
            expect(JSON.stringify(actual)).toEqual(JSON.stringify(expected));
        });
    });

    it('validates minimal correct arguments with appclass', function () {
        let input = {
            appname: 'Smoke Grinder',
            author: 'Joe Coder <joe@coder.com>',
            appversion: '1.0',
            appclass: 'SmokeGrinder.App',
        };
        expect(Module.validateRemainingArgs(input)).toBeTruthy();
    });

    it('validates minimal correct arguments with exec', function () {
        let input = {
            appname: 'Smoke Grinder',
            author: 'Joe Coder <joe@coder.com>',
            appversion: '1.0',
            exec: 'bin/smoke-grinder-launch',
        };
        expect(Module.validateRemainingArgs(input)).toBeTruthy();
    });

    it('validates maximal correct arguments', function () {
        let input = {
            appname: 'Smoke Grinder',
            author: 'Joe Coder <joe@coder.com>',
            appversion: '1.0',
            appclass: 'SmokeGrinder.App',
            'manifest-version': '0',
            website: 'http://coder.example.com',
            description: 'An app that does exciting things',
            locale: 'en',
            license: 'GPL',
        };
        expect(Module.validateRemainingArgs(input)).toBeTruthy();
    });

    it('requires an appname', function () {
        let input = {
            author: 'Joe Coder <joe@coder.com>',
            appversion: '1.0',
            appclass: 'SmokeGrinder.App',
        };
        expect(Module.validateRemainingArgs(input)).toBeFalsy();
    });

    it('requires an author', function () {
        let input = {
            appname: 'Smoke Grinder',
            appversion: '1.0',
            appclass: 'SmokeGrinder.App',
        };
        expect(Module.validateRemainingArgs(input)).toBeFalsy();
    });

    it('requires an app version', function () {
        let input = {
            appname: 'Smoke Grinder',
            author: 'Joe Coder <joe@coder.com>',
            appclass: 'SmokeGrinder.App',
        };
        expect(Module.validateRemainingArgs(input)).toBeFalsy();
    });

    it('requires a launcher item', function () {
        let input = {
            appname: 'Smoke Grinder',
            author: 'Joe Coder <joe@coder.com>',
            appversion: '1.0',
        };
        expect(Module.validateRemainingArgs(input)).toBeFalsy();
    });

    it('does not allow more than one launcher item', function () {
        let input = {
            appname: 'Smoke Grinder',
            author: 'Joe Coder <joe@coder.com>',
            appversion: '1.0',
            appclass: 'SmokeGrinder.App',
            exec: 'bin/smoke-grinder-launch',
        };
        expect(Module.validateRemainingArgs(input)).toBeFalsy();
    });

    it('chokes on unknown arguments', function () {
        let input = {
            appname: 'Smoke Grinder',
            author: 'Joe Coder <joe@coder.com>',
            appversion: '1.0',
            appclass: 'SmokeGrinder.App',
            unrecognized_argument: 'a value',
        };
        expect(Module.validateRemainingArgs(input)).toBeFalsy();
    });

    it('knows when arguments should have values', function () {
        let input = {
            appname: null,
            author: null,
            appversion: null,
            appclass: null,
        };
        expect(Module.validateRemainingArgs(input)).toBeFalsy();
    });

    it('rejects a bad manifest version', function () {
        let input = {
            appname: 'Smoke Grinder',
            author: 'Joe Coder <joe@coder.com>',
            appversion: '1.0',
            appclass: 'SmokeGrinder.App',
            'manifest-version': 'FF',
        };
        expect(Module.validateRemainingArgs(input)).toBeFalsy();
    });

    it('rejects a manifest version that is too high', function () {
        let input = {
            appname: 'Smoke Grinder',
            author: 'Joe Coder <joe@coder.com>',
            appversion: '1.0',
            appclass: 'SmokeGrinder.App',
            'manifest-version': 99999,
        };
        expect(Module.validateRemainingArgs(input)).toBeFalsy();
    });

    it('creates the correct filename for the manifest', function () {
        let input = {
            applicationId: 'com.endlessm.smoke-grinder',
        };
        expect(Module.createFilenameForManifest(input))
            .toEqual('com.endlessm.smoke-grinder.json');
    });

    it('creates a whole manifest from minimal args, with appclass', function () {
        let inputApplicationId = 'com.coder.smoke-grinder';
        let inputArgDict = {
            appname: 'Smoke Grinder',
            author: 'Joe Coder <joe@coder.com>',
            appversion: '1.0.1',
            appclass: 'SmokeGrinder.App',
        };
        let expected = {
            manifestVersion: 1,
            applicationId: 'com.coder.smoke-grinder',
            applicationName: {
                en: 'Smoke Grinder',
            },
            authorName: 'Joe Coder <joe@coder.com>',
            authorWebsite: '',
            description: {
                en: '',
            },
            version: '1.0.1',
            changes: {
                en: [],
            },
            license: '',
            resources: [],
            applicationClass: 'SmokeGrinder.App',
            icons: {},
            categories: [],
            permissions: [],
            metadata: {},
        };
        let actual = Module.createManifest(inputApplicationId, inputArgDict);
        expect(JSON.stringify(actual)).toEqual(JSON.stringify(expected));
    });

    it('creates a whole manifest from minimal args, with exec', function () {
        let inputApplicationId = 'com.coder.smoke-grinder';
        let inputArgDict = {
            appname: 'Smoke Grinder',
            author: 'Joe Coder <joe@coder.com>',
            appversion: '1.0.1',
            exec: 'bin/smoke-grinder-launch',
        };
        let expected = {
            manifestVersion: 1,
            applicationId: 'com.coder.smoke-grinder',
            applicationName: {
                en: 'Smoke Grinder',
            },
            authorName: 'Joe Coder <joe@coder.com>',
            authorWebsite: '',
            description: {
                en: '',
            },
            version: '1.0.1',
            changes: {
                en: [],
            },
            license: '',
            resources: [],
            exec: 'bin/smoke-grinder-launch',
            icons: {},
            categories: [],
            permissions: [],
            metadata: {},
        };
        let actual = Module.createManifest(inputApplicationId, inputArgDict);
        expect(JSON.stringify(actual)).toEqual(JSON.stringify(expected));
    });

    it('creates a whole manifest from maximal args', function () {
        let inputApplicationId = 'com.coder.smoke-grinder';
        let inputArgDict = {
            appname: 'Smoke Grinder',
            author: 'Joe Coder <joe@coder.com>',
            appversion: '1.0.1',
            appclass: 'SmokeGrinder.App',
            'manifest-version': '0',
            website: 'http://coder.example.com',
            description: 'An app that does exciting things',
            locale: 'pt_BR',
            license: 'GPL'
        };
        let expected = {
            manifestVersion: 0,
            applicationId: 'com.coder.smoke-grinder',
            applicationName: {
                pt_BR: 'Smoke Grinder'
            },
            authorName: 'Joe Coder <joe@coder.com>',
            authorWebsite: 'http://coder.example.com',
            description: {
                pt_BR: 'An app that does exciting things'
            },
            version: '1.0.1',
            changes: {
                pt_BR: [],
            },
            license: 'GPL',
            resources: [],
            applicationClass: 'SmokeGrinder.App',
            icons: {},
            categories: [],
            permissions: [],
            metadata: {}
        };
        let actual = Module.createManifest(inputApplicationId, inputArgDict);
        expect(JSON.stringify(actual)).toEqual(JSON.stringify(expected));
    });

    it('reports a subcommand summary for use in help', function () {
        expect(typeof Module.summary()).toEqual('string');
    });
});
