const std = @import("std");
const builtin = @import("builtin");

comptime
{
    if (builtin.zig_version.minor < 13)
        @compileError("building watte with Zig required at leats Zig 0.13.0");
}

fn compileBinary(b: *std.Build, name: []const u8, rootdir: []const u8, flags: []const []const u8,
                 sources: []const []const u8, target: std.Build.ResolvedTarget,
                 optimize: std.builtin.OptimizeMode) !*std.Build.Step.Compile
{
    const binary = b.addExecutable(
        .{
            .name = name,
            .target = target,
            .optimize = optimize,
        }
    );
    var build_flags: std.ArrayListUnmanaged([]const u8) = .{};
    var cpp_sources: std.ArrayListUnmanaged([]const u8) = .{};

    try build_flags.appendSlice(b.allocator, flags);
    try cpp_sources.appendSlice(b.allocator, sources);

    switch (target.result.os.tag)
    {
        .linux => {
        },
        .freebsd => {
        },
        .netbsd => {
        },
        .openbsd => {
        },
        .dragonfly => {
        },
        .macos => {
        },
        else => {
            @panic("unsupported os");
        },
    }

    binary.addIncludePath(b.path(rootdir));
    binary.root_module.addCSourceFiles(
        .{
            .root = b.path(rootdir),
            .files = cpp_sources.items,
            .flags = build_flags.items,
        }
    );
    binary.linkLibCpp();
    binary.linkSystemLibrary("ncurses");

    return binary;
}

const shared_flags = &[_][]const u8{
    "-std=c++17",
    "-flto",
    "-W", "-Wall", "-Wextra",
    "-Os",
};

const sources_watte = &[_][]const u8{
    "watte.cxx",
};

pub fn build(b: *std.Build) !void
{
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});
    const watte_binary = try compileBinary(b, "watte", "./", shared_flags, sources_watte, target,
                                           optimize);
    const run_watte = b.addRunArtifact(watte_binary);
    const run_step_watte = b.step("run", "Run watte application");

    run_step_watte.dependOn(&run_watte.step);

    b.installArtifact(watte_binary);
}
