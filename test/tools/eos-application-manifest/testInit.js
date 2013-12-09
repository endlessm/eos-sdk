const Module = imports.tools['eos-application-manifest'].commands.init;

function testParseOneArgumentWithoutValue() {
    let input = ['--parameter'];
    let expected = { parameter: null };
    let actual = Module.parseRemainingArgs(input);
    assertEquals(JSON.stringify(expected), JSON.stringify(actual));
}

function testParseOneArgumentWithValue() {
    let input = ['--parameter=value'];
    let expected = { parameter: "value" };
    let actual = Module.parseRemainingArgs(input);
    assertEquals(JSON.stringify(expected), JSON.stringify(actual));
}

function testParseOneArgumentFollowedByValue() {
    let input = ['--parameter', 'value'];
    let expected = { parameter: "value" };
    let actual = Module.parseRemainingArgs(input);
    assertEquals(JSON.stringify(expected), JSON.stringify(actual));
}

function testParseTwoArgumentsWithoutValue() {
    let input = ['--parameter-one', '--parameter-two'];
    let expected = { 'parameter-one': null, 'parameter-two': null };
    let actual = Module.parseRemainingArgs(input);
    assertEquals(JSON.stringify(expected), JSON.stringify(actual));
}

function testParseArgumentWithValueFollowedByArgument() {
    let input = ['--parameter-one=value', '--parameter-two'];
    let expected = { 'parameter-one': 'value', 'parameter-two': null };
    let actual = Module.parseRemainingArgs(input);
    assertEquals(JSON.stringify(expected), JSON.stringify(actual));
}

function testParseArgumentFollowedByValueFollowedByArgument() {
    let input = ['--parameter-one', 'value', '--parameter-two'];
    let expected = { 'parameter-one': 'value', 'parameter-two': null };
    let actual = Module.parseRemainingArgs(input);
    assertEquals(JSON.stringify(expected), JSON.stringify(actual));
}

function testValidateMinimalCorrectArgumentsWithAppclass() {
    let input = {
        appname: 'Smoke Grinder',
        author: 'Joe Coder <joe@coder.com>',
        appversion: '1.0',
        appclass: 'SmokeGrinder.App'
    };
    assertTrue(Module.validateRemainingArgs(input));
}

function testValidateMinimalCorrectArgumentsWithExec() {
    let input = {
        appname: 'Smoke Grinder',
        author: 'Joe Coder <joe@coder.com>',
        appversion: '1.0',
        exec: 'bin/smoke-grinder-launch'
    };
    assertTrue(Module.validateRemainingArgs(input));
}

function testValidateMaximalCorrectArguments() {
    let input = {
        appname: 'Smoke Grinder',
        author: 'Joe Coder <joe@coder.com>',
        appversion: '1.0',
        appclass: 'SmokeGrinder.App',
        'manifest-version': '0',
        website: 'http://coder.example.com',
        description: 'An app that does exciting things',
        locale: 'en',
        license: 'GPL'
    };
    assertTrue(Module.validateRemainingArgs(input));
}

function testValidateBadArgumentsAppnameMissing() {
    let input = {
        author: 'Joe Coder <joe@coder.com>',
        appversion: '1.0',
        appclass: 'SmokeGrinder.App'
    };
    assertFalse(Module.validateRemainingArgs(input));
}

function testValidateBadArgumentsAuthorMissing() {
    let input = {
        appname: 'Smoke Grinder',
        appversion: '1.0',
        appclass: 'SmokeGrinder.App'
    };
    assertFalse(Module.validateRemainingArgs(input));
}

function testValidateBadArgumentsAppversionMissing() {
    let input = {
        appname: 'Smoke Grinder',
        author: 'Joe Coder <joe@coder.com>',
        appclass: 'SmokeGrinder.App'
    };
    assertFalse(Module.validateRemainingArgs(input));
}

function testValidateBadArgumentsLauncherMissing() {
    let input = {
        appname: 'Smoke Grinder',
        author: 'Joe Coder <joe@coder.com>',
        appversion: '1.0',
    };
    assertFalse(Module.validateRemainingArgs(input));
}

function testValidateBadArgumentsTooManyLaunchers() {
    let input = {
        appname: 'Smoke Grinder',
        author: 'Joe Coder <joe@coder.com>',
        appversion: '1.0',
        appclass: 'SmokeGrinder.App',
        exec: 'bin/smoke-grinder-launch'
    };
    assertFalse(Module.validateRemainingArgs(input));
}

function testValidateBadUnknownArgument() {
    let input = {
        appname: 'Smoke Grinder',
        author: 'Joe Coder <joe@coder.com>',
        appversion: '1.0',
        appclass: 'SmokeGrinder.App',
        unrecognized_argument: 'a value'
    };
    assertFalse(Module.validateRemainingArgs(input));
}

function testValidateBadArgumentsWithoutValues() {
    let input = {
        appname: null,
        author: null,
        appversion: null,
        appclass: null
    };
    assertFalse(Module.validateRemainingArgs(input));
}

function testValidateBadManifestVersion() {
    let input = {
        appname: 'Smoke Grinder',
        author: 'Joe Coder <joe@coder.com>',
        appversion: '1.0',
        appclass: 'SmokeGrinder.App',
        'manifest-version': 'FF'
    };
    assertFalse(Module.validateRemainingArgs(input));
}

function testValidateTooHighManifestVersion() {
    let input = {
        appname: 'Smoke Grinder',
        author: 'Joe Coder <joe@coder.com>',
        appversion: '1.0',
        appclass: 'SmokeGrinder.App',
        'manifest-version': 99999
    };
    assertFalse(Module.validateRemainingArgs(input));
}

function testCreateFilenameForManifest() {
    let input = {
        applicationId: 'com.endlessm.smoke-grinder'
    };
    let expected = 'com.endlessm.smoke-grinder.json';
    let actual = Module.createFilenameForManifest(input);
}

function testCreateManifestFromMinimalArgsWithAppclass() {
    let inputApplicationId = 'com.coder.smoke-grinder';
    let inputArgDict = {
        appname: 'Smoke Grinder',
        author: 'Joe Coder <joe@coder.com>',
        appversion: '1.0.1',
        appclass: 'SmokeGrinder.App'
    };
    let expected = {
        manifestVersion: 1,
        applicationId: "com.coder.smoke-grinder",
        applicationName: {
            en: "Smoke Grinder"
        },
        authorName: "Joe Coder <joe@coder.com>",
        authorWebsite: "",
        description: {
            en: ""
        },
        version: "1.0.1",
        changes: {
            en: [],
        },
        license: "",
        resources: [],
        applicationClass: "SmokeGrinder.App",
        icons: {},
        categories: [],
        permissions: [],
        metadata: {}
    };
    let actual = Module.createManifest(inputApplicationId, inputArgDict);
    assertEquals(JSON.stringify(expected), JSON.stringify(actual));
}

function testCreateManifestFromMinimalArgsWithExec() {
    let inputApplicationId = 'com.coder.smoke-grinder';
    let inputArgDict = {
        appname: 'Smoke Grinder',
        author: 'Joe Coder <joe@coder.com>',
        appversion: '1.0.1',
        exec: 'bin/smoke-grinder-launch'
    };
    let expected = {
        manifestVersion: 1,
        applicationId: "com.coder.smoke-grinder",
        applicationName: {
            en: "Smoke Grinder"
        },
        authorName: "Joe Coder <joe@coder.com>",
        authorWebsite: "",
        description: {
            en: ""
        },
        version: "1.0.1",
        changes: {
            en: [],
        },
        license: "",
        resources: [],
        exec: "bin/smoke-grinder-launch",
        icons: {},
        categories: [],
        permissions: [],
        metadata: {}
    };
    let actual = Module.createManifest(inputApplicationId, inputArgDict);
    assertEquals(JSON.stringify(expected), JSON.stringify(actual));
}

function testCreateManifestFromMaximalArgs() {
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
        applicationId: "com.coder.smoke-grinder",
        applicationName: {
            pt_BR: "Smoke Grinder"
        },
        authorName: "Joe Coder <joe@coder.com>",
        authorWebsite: "http://coder.example.com",
        description: {
            pt_BR: "An app that does exciting things"
        },
        version: "1.0.1",
        changes: {
            pt_BR: [],
        },
        license: "GPL",
        resources: [],
        applicationClass: "SmokeGrinder.App",
        icons: {},
        categories: [],
        permissions: [],
        metadata: {}
    };
    let actual = Module.createManifest(inputApplicationId, inputArgDict);
    assertEquals(JSON.stringify(expected), JSON.stringify(actual));
}

function testSummary() {
    let summary = Module.summary();
    assertTrue(typeof summary == 'string');
}
