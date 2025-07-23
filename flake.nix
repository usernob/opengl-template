{
  description = "simple c/c+ dev environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
    }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
      {
        devShell = pkgs.mkShell rec {
          name = "opengl";

          nativeBuildInputs = with pkgs; [
            cmake
            ninja
            pkg-config
            libgcc
            wayland-scanner
          ];

          buildInputs = with pkgs; [
            libGL.dev
            libxkbcommon
            xorg.libX11.dev
            xorg.libXrandr.dev
            xorg.libXext.dev
            xorg.libXcursor.dev
            xorg.libXinerama.dev
            xorg.libXi.dev
            xorg.libXfixes.dev
            xorg.libxcb
            wayland
            libffi.dev
          ];

          packages = with pkgs; [
            cmake-lint
            cmake-format
            neocmakelsp
            clang-tools
            valgrind
            gdb
            glsl_analyzer
          ];
          LD_LIBRARY_PATH = pkgs.lib.makeLibraryPath buildInputs;
        };
      }
    );
}
