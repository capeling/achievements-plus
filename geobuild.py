# These lines are optional, but they will provide working Python intellisense,
# once you build your project at least once.
from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from .build.geobuild.prelude import *

def main(build: Build):
    debug = build.add_option('DEBUG_BUILD')

    build.add_include_dir("src")
    build.add_source_dir("src/*.cpp", recursive=True)

    if debug:
        build.add_definition('DEBUG_BUILD', '1')

    build.add_cpm_dep("dankmeme01/cue", "e80ac94", link_name="cue")