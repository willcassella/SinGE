// main.js
print("Inside 'main.js'\n");

process_entities_mut("sge::CTransform3D", "sge::CPerspectiveCamera",
    (pframe, entity, transform, camera) => {
        camera.h_fov = 90;
    }
);
