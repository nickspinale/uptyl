{ stdenv }:

stdenv.mkDerivation {
  name = "uttyl";
  builder = builtins.toFile "builder.sh" ''
    source $stdenv/setup
    mkdir -p $out/bin
    gcc -lutil -o $out/bin/parpty ${./parpty.c}
    gcc -lutil -o $out/bin/tmuxdo ${./tmuxdo.c}
    cp ${./urxvtdo} $out/bin/urxvtdo
    cp ${./tmuxtry} $out/bin/tmuxtry
  '';
}
