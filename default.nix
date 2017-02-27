{ stdenv }:

stdenv.mkDerivation {
  name = "uptyl";
  builder = builtins.toFile "builder.sh" ''
    source $stdenv/setup
    mkdir -p $out/bin
    gcc -lutil -o $out/bin/parpty ${./parpty.c}
  '';
}
