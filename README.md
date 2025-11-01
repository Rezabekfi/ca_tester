# ca tester


bool ConvexHull::edge_center(uint8_t current_state,
                             const RuleContext& ctx,
                             const std::vector<uint8_t>& neighbours) const {
    uint8_t dist_x = get_distance(current_state);
    auto deltas = pick_deltas(ctx.neighborhood);
    const auto& gridVals = ctx.grid.getGridValues();

    for (std::size_t i = 0; i < deltas.size(); ++i) {
        int nx = static_cast<int>(ctx.x) + deltas[i].first;
        int ny = static_cast<int>(ctx.y) + deltas[i].second;
        if (nx < 0 || ny < 0 ||
            nx >= static_cast<int>(ctx.grid.getWidth()) ||
            ny >= static_cast<int>(ctx.grid.getHeight())) continue;

        uint8_t neigh_state = gridVals[idx(nx, ny, ctx.grid.getWidth())];
        float dist_xy = std::min<float>(dist_x, get_distance(neigh_state)) + 0.5f;

        // Build N(xy)
        auto edge_nb = ctx.getEdgeNeighborhood(ctx.x, ctx.y,
                                               static_cast<std::size_t>(nx),
                                               static_cast<std::size_t>(ny));

        // Check for at least one cell near dist_xy - 1.5
        for (uint8_t z : edge_nb) {
            if (std::abs(static_cast<float>(get_distance(z)) - (dist_xy - 1.5f)) < 1e-3f)
                return true;
        }
    }
    return false;
}

